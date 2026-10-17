#pragma once
#include "CoreMinimal.h"

#include <atomic>
#include <memory>
#include <string>
#include <thread>

/** Configuration for the local backend HTTP server. */
struct FBackendHttpServerSettings
{
	/** Interface to bind to. Keep this on loopback unless you really want it exposed. */
	std::string Address = "127.0.0.1";

	/** Port to listen on. */
	int32 Port = 8088;

	/** Shared secret expected in the "Authorization: Bearer <Secret>" header. */
	std::string Secret = "welcome-to-bp";

	/** Max request body size in bytes. */
	size_t MaxPayloadLength = 4096;
};

/**
 * Small admin HTTP server the matchmaking backend talks to.
 * The listen loop runs on its own thread; handlers run on httplib's worker pool.
 */
class FBackendHttpServer
{
public:
	/** Returns the process-wide server instance. Intentionally leaked so nothing joins threads under the loader lock at DLL unload. */
	static FBackendHttpServer& Get();

	/** Binds synchronously and starts listening in the background. Returns false if already running or the bind failed. */
	bool Start(const FBackendHttpServerSettings& InSettings = FBackendHttpServerSettings());

	/** Stops listening and waits for the listen thread. Must NOT be called from inside a request handler. */
	void Stop();

	bool IsRunning() const { return bIsRunning.load(std::memory_order_acquire); }

	FBackendHttpServer(const FBackendHttpServer&) = delete;
	FBackendHttpServer& operator=(const FBackendHttpServer&) = delete;

private:
	FBackendHttpServer();
	~FBackendHttpServer();

	struct FImpl;
	std::unique_ptr<FImpl> Impl;

	FBackendHttpServerSettings Settings;
	std::thread ListenThread;
	std::atomic<bool> bIsRunning{ false };
};
