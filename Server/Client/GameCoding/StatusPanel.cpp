#include "pch.h"
#include "StatusPanel.h"
#include "ResourceManager.h"
#include "Button.h"
#include "InputManager.h"
#include "NetworkManager.h"

StatusPanel::StatusPanel(NetworkManager* networkManager) : _networkManager(networkManager)
{
    auto ui = make_unique<Button>();
    ui->SetSprite(GET_SINGLE(ResourceManager)->GetSprite(L"Start_Off"), BS_Default);
    ui->SetSprite(GET_SINGLE(ResourceManager)->GetSprite(L"Start_On"), BS_Clicked);
    ui->SetPos({ 800, 200 });
    ui->AddOnClickDelegate(this, &StatusPanel::OnClickReconnectButton);
    AddChild(move(ui));
}

StatusPanel::~StatusPanel()
{
}

void StatusPanel::BeginPlay()
{
    Super::BeginPlay();
}

void StatusPanel::Tick()
{
    Super::Tick();

    POINT mousePos = GET_SINGLE(InputManager)->GetMousePos();
    RECT ipRect = { 100, 300, 300, 330 };
    RECT portRect = { 100, 340, 300, 370 };
    if (GET_SINGLE(InputManager)->GetButtonDown(KeyType::LeftMouse))
    {
        if (PtInRect(&ipRect, mousePos))
        {
            _editingIp = true;
            _editingPort = false;
        }
        else if (PtInRect(&portRect, mousePos))
        {
            _editingIp = false;
            _editingPort = true;
        }
        else
        {
            _editingIp = false;
            _editingPort = false;
        }
    }

    auto captureText = [](wstring& text, bool allowDot)
        {
            if ((GetAsyncKeyState(VK_BACK) & 0x0001) && !text.empty())
                text.pop_back();

            for (int vk = '0'; vk <= '9'; ++vk)
            {
                if (GetAsyncKeyState(vk) & 0x0001)
                    text.push_back(static_cast<wchar_t>(vk));
            }

            if (allowDot && (GetAsyncKeyState(VK_OEM_PERIOD) & 0x0001))
                text.push_back(L'.');
        };

    if (_editingIp)
        captureText(_serverIp, true);
    else if (_editingPort)
        captureText(_serverPort, false);
}

void StatusPanel::Render(HDC hdc)
{
    Super::Render(hdc);

    wstring status = L"Disconnected";
    if (_networkManager && _networkManager->IsConnected())
        status = L"Connected";
    Utils::DrawTextW(hdc, { 100, 50 }, std::format(L"Status : {0}", status));
    Utils::DrawTextW(hdc, { 100, 300 }, std::format(L"IP : {0}", _serverIp));
    Utils::DrawTextW(hdc, { 100, 340 }, std::format(L"Port : {0}", _serverPort));
}

void StatusPanel::OnClickReconnectButton()
{
    if (_networkManager)
    {
        uint16 port = static_cast<uint16>(_wtoi(_serverPort.c_str()));
        _networkManager->SetServerInfo(_serverIp, port);
        _networkManager->RequestReconnect();
    }
}
