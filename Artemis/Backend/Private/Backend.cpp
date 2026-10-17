#include "pch.h"
#include "Backend/Public/Backend.h"

void Backend::LoadModule()
{
	FBackendHttpServer::Get().Start();
}
