#include "pch.h"
#include "Backend/Public/BackendHttpServer.h"
#include "Backend/Public/BackendVars.h"

#include "httplib.h"
#include "json.hpp"

namespace BackendHttpServer
{
	static void SendJson(httplib::Response& Response, int32 Status, const nlohmann::json& Body)
	{
		Response.status = Status;
		Response.set_content(Body.dump(), "application/json");
	}

	static void SendError(httplib::Response& Response, int32 Status, const char* Message)
	{
		SendJson(Response, Status, { { "error", Message } });
	}

	static bool ConstantTimeEquals(const std::string& A, const std::string& B)
	{
		if (A.size() != B.size())
		{
			return false;
		}

		uint8 Diff = 0;
		for (size_t Index = 0; Index < A.size(); ++Index)
		{
			Diff |= static_cast<uint8>(A[Index] ^ B[Index]);
		}
		return Diff == 0;
	}
}

struct FBackendHttpServer::FImpl
{
	httplib::Server Server;

	explicit FImpl(FBackendHttpServer& Owner)
	{
		Server.Post("/admin/v1/player-count", [&Owner](const httplib::Request& Request, httplib::Response& Response)
		{
			HandlePlayerCount(Owner, Request, Response);
		});

		Server.set_exception_handler([](const httplib::Request& Request, httplib::Response& Response, std::exception_ptr Exception)
		{
			UE_LOG(LogBackend, Error, TEXT("FBackendHttpServer: Unhandled exception while serving %hs %hs"), Request.method.c_str(), Request.path.c_str());
			BackendHttpServer::SendError(Response, 500, "internal error");
		});
	}

	static bool IsAuthorized(const FBackendHttpServer& Owner, const httplib::Request& Request)
	{
		const std::string Header = Request.get_header_value("Authorization");
		return BackendHttpServer::ConstantTimeEquals(Header, "Bearer " + Owner.Settings.Secret);
	}

	static void HandlePlayerCount(FBackendHttpServer& Owner, const httplib::Request& Request, httplib::Response& Response)
	{
		if (!IsAuthorized(Owner, Request))
		{
			UE_LOG(LogBackend, Warning, TEXT("FBackendHttpServer::HandlePlayerCount: Rejected unauthorized request from %hs"), Request.remote_addr.c_str());
			BackendHttpServer::SendError(Response, 401, "unauthorized");
			return;
		}

		const nlohmann::json Json = nlohmann::json::parse(Request.body, nullptr, /*allow_exceptions*/ false);
		if (Json.is_discarded())
		{
			BackendHttpServer::SendError(Response, 400, "invalid JSON");
			return;
		}

		const auto CountIt = Json.is_object() ? Json.find("humanPlayersCount") : Json.end();
		if (CountIt == Json.end() || !CountIt->is_number_integer())
		{
			BackendHttpServer::SendError(Response, 400, "humanPlayersCount must be an integer");
			return;
		}

		const int64 Count = CountIt->get<int64>();
		// At least one human: the game only continues WaitForMatchAssignmentReady when the count is > 0.
		if (Count < 1 || Count > 100)
		{
			BackendHttpServer::SendError(Response, 400, "player count out of range");
			return;
		}

		BackendVars::PlayerBots::CachedHumanPlayersCount.store(static_cast<int32>(Count), std::memory_order_relaxed);
		BackendVars::PlayerBots::bHumanPlayersCountReceived.store(true, std::memory_order_release);

		UE_LOG(LogBackend, Log, TEXT("FBackendHttpServer::HandlePlayerCount: CachedHumanPlayersCount = %lld"), Count);

		BackendHttpServer::SendJson(Response, 200, { { "success", true }, { "humanPlayersCount", Count } });
		Response.set_header("Connection", "close");

		UE_LOG(LogBackend, Log, TEXT("FBackendHttpServer::HandlePlayerCount: Player count received, shutting down"));
		Owner.Impl->Server.stop();
	}
};

FBackendHttpServer& FBackendHttpServer::Get()
{
	static FBackendHttpServer* Instance = new FBackendHttpServer();
	return *Instance;
}

FBackendHttpServer::FBackendHttpServer()
	: Impl(std::make_unique<FImpl>(*this))
{
}

FBackendHttpServer::~FBackendHttpServer()
{
	Stop();
}

bool FBackendHttpServer::Start(const FBackendHttpServerSettings& InSettings)
{
	if (InSettings.Secret.empty() || InSettings.Port < 1 || InSettings.Port > 65535)
	{
		UE_LOG(LogBackend, Error, TEXT("FBackendHttpServer::Start: Invalid settings (empty secret or port %d out of range)"), InSettings.Port);
		return false;
	}

	bool bExpected = false;
	if (!bIsRunning.compare_exchange_strong(bExpected, true, std::memory_order_acq_rel))
	{
		UE_LOG(LogBackend, Warning, TEXT("FBackendHttpServer::Start: Already running"));
		return false;
	}

	if (ListenThread.joinable())
	{
		ListenThread.join();
	}

	Settings = InSettings;
	Impl->Server.set_payload_max_length(Settings.MaxPayloadLength);

	if (!Impl->Server.bind_to_port(Settings.Address, Settings.Port))
	{
		UE_LOG(LogBackend, Error, TEXT("FBackendHttpServer::Start: Failed to bind %hs:%d (port already in use?)"), Settings.Address.c_str(), Settings.Port);
		bIsRunning.store(false, std::memory_order_release);
		return false;
	}

	ListenThread = std::thread([this]()
	{
		if (!Impl->Server.listen_after_bind())
		{
			UE_LOG(LogBackend, Error, TEXT("FBackendHttpServer: Listen loop exited with an error"));
		}

		bIsRunning.store(false, std::memory_order_release);
	});

	UE_LOG(LogBackend, Log, TEXT("FBackendHttpServer::Start: Listening on http://%hs:%d"), Settings.Address.c_str(), Settings.Port);
	return true;
}

void FBackendHttpServer::Stop()
{
	Impl->Server.stop();

	if (ListenThread.joinable())
	{
		ListenThread.join();
	}

	bIsRunning.store(false, std::memory_order_release);
}
