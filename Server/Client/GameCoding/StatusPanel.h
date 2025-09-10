#pragma once

#include "Panel.h"

class StatusPanel : public Panel
{
    using Super = Panel;
public:
    explicit StatusPanel(NetworkManager* networkManager);
    virtual ~StatusPanel() override;

    virtual void BeginPlay() override;
    virtual void Tick() override;
    virtual void Render(HDC hdc) override;

    void OnClickReconnectButton();

private:
    NetworkManager* _networkManager = nullptr;
    wstring _serverIp = L"127.0.0.1";
    wstring _serverPort = L"7777";
    bool _editingIp = false;
    bool _editingPort = false;
};

