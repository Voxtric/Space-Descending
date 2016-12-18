
//Title     : GamepadClass.h
//Purpose   : GamepadClass header.
//Author    : Preston Phillips
//Date      : 29/10/13

#pragma once

#include<Windows.h>     //Needed for windows
#include<XInput.h>      //gamepad related code
#include"vector2D.h"    //Used for stick locations

//Xinput lib
#pragma comment(lib, "XInput.lib") 

using namespace std;

//Created with the assistance of
//http://msdn.microsoft.com/en-us/library/windows/desktop/ee417001%28v=vs.85%29.aspx
//http://katyscode.wordpress.com/2013/08/30/xinput-tutorial-part-1-adding-gamepad-support-to-your-windows-game/



const int GAMEPAD_TOTAL_BUTTONS=14;
const int GAMEPAD_TOTAL_GAMEPADS_POSSABLE=4;

//The buttons on the gamepad
enum GamepadButtonTag{GPB_A,GPB_B,GPB_X,GPB_Y,
                      GPB_START,GPB_SELECT,
                      GPB_LB,GPB_RB,
                      GPB_LS,GPB_RS,
                      GPB_DPU,GPB_DPD,
                      GPB_DPL,GPB_DPR};

class GamepadClass
{
    public:
        //Create a GamepadClass
        explicit GamepadClass(int iId);
        ~GamepadClass(void);

        //Set ALL button Values to not being used
        void SetStartUpValues();

        //Called every frame to get gamepads state
        void SampleGamepad();

        //Get info on gamepad
        int GetPort() const;
        int GetId() const;
        bool IsActive();

        //Set info on gamepad
        void SetActive(bool bThis);

        //DeadZones
        //Gets
        float GetLeftStickDeadZone();
        float GetLeftTriggerDeadZone();
        float GetRightStickDeadZone();
        float GetRightTriggerDeadZone();
        //Sets
        void SetLeftStickDeadZone(float fThis);
        void SetLeftTriggerDeadZone(float fThis);
        void SetRightStickDeadZone(float fThis);
        void SetRightTriggerDeadZone(float fThis);

        //Get Stick Values
        //Stick vector2D are from -1 to 1 in x+y
        //Get % from deadzone in the direction the stick has moved
        Vector2D GetLeftStick();
        Vector2D GetRightStick();
        //Raw gets the % the stick is in that direction regardles of deadzone
        Vector2D GetLeftStickRaw();
        Vector2D GetRightStickRaw();
        bool IsInDeadZoneLeftStick();
        bool IsInDeadZoneRightStick();

        //Get Trigger values
        //Gets % of trigger is pressed outside the deadzone
        float GetLeftTrigger();
        float GetRightTrigger();
        //Raw gets is % trigger is pressed regardles of deadzone
        float GetLeftTriggerRaw();
        float GetRightTriggerRaw();
        bool IsInDeadZoneLeftTrigger();
        bool IsInDeadZoneRightTrigger();


        //Get A buttons state
        bool ButtonPressed(GamepadButtonTag gpbTag);
        bool ButtonJustPressed(GamepadButtonTag gpbTag);

    private:
        //Called in SampleGamepad() to create m_state info
        bool IsConnected();

        //Gamepad Info
        bool m_bActive;
        int m_iId;
        XINPUT_STATE m_state;
        
        //DeadZone
        float m_fLeftStickDeadZone;
        float m_fLeftTriggerDeadZone;
        float m_fRightStickDeadZone;
        float m_fRightTriggerDeadZone;

        //ButtonStats
        Vector2D m_v2dLeftStick;
        Vector2D m_v2dRightStick;
        float m_fLeftTrigger;
        float m_fRightTrigger;
        
        bool m_abCurrentButtonsDown[GAMEPAD_TOTAL_BUTTONS];
        bool m_abPreviousButtonsDown[GAMEPAD_TOTAL_BUTTONS];
};

//Called to draw a test for a gamepad that shows states
void GamepadInputTest(GamepadClass* theGamepad, Vector2D v2dLocation);