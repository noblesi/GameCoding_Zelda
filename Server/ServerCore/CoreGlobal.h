#pragma once

class ThreadManager;

unique_ptr<ThreadManager> GThreadManager = make_unique<ThreadManager>();