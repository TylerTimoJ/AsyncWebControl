/*
*   Contains definitions for base PageElement class which is used during the building of the webpage
*   Also contains definitions for the various elements including controls and displays
*   Each elemenet must override the pure virtual functions of tht PageElement base class
*/

#ifndef CONTROLELEMENT_H
#define CONTROLELEMENT_H

#include <string>
#include <vector>
#include <iostream>
#include <Arduino.h>

class PageElement
{
public:
    struct EEPROM_Data
    {
        uint8_t data[4];
    };

    EEPROM_Data m_EEPROM_Packtet;

    std::string m_title;
    std::string m_ID;

    PageElement(std::string title);

    virtual void GetHTML(std::ostringstream &stream) = 0;
    virtual void GetJavaCommandFunction(std::ostringstream &stream) = 0;
    virtual void GetJavaOnMessageStatment(std::ostringstream &stream) = 0;
    virtual std::string GetSocketMessage() = 0;
    virtual bool ProcessSocketMessage(std::string message) = 0;
    virtual void InitializeData(EEPROM_Data data) = 0;
    virtual EEPROM_Data GetEEPROMData();
};

class Input_Toggle : public PageElement
{
private:
    uint8_t *m_data;

public:
    Input_Toggle(std::string title, uint8_t *controlVar);

    void GetHTML(std::ostringstream &stream) override;
    void GetJavaCommandFunction(std::ostringstream &stream) override;
    void GetJavaOnMessageStatment(std::ostringstream &stream) override;
    std::string GetSocketMessage() override;
    bool ProcessSocketMessage(std::string message) override;
    void InitializeData(EEPROM_Data data) override;
    //EEPROM_Data GetEEPROMData() override;
};

class Input_Slider : public PageElement
{
private:
    uint8_t *m_data;
    uint8_t m_max;

public:
    Input_Slider(std::string title, uint8_t *controlVar, uint8_t max);

    void GetHTML(std::ostringstream &stream) override;
    void GetJavaCommandFunction(std::ostringstream &stream) override;
    void GetJavaOnMessageStatment(std::ostringstream &stream) override;
    std::string GetSocketMessage() override;
    bool ProcessSocketMessage(std::string message) override;
    void InitializeData(EEPROM_Data data) override;
};

class Input_DropDown : public PageElement
{
private:
    std::vector<std::string> m_Selections;
    uint8_t *m_data;

public:
    Input_DropDown(std::string title, uint8_t *controlVar);

    void GetHTML(std::ostringstream &stream) override;
    void GetJavaCommandFunction(std::ostringstream &stream) override;
    void GetJavaOnMessageStatment(std::ostringstream &stream) override;
    std::string GetSocketMessage() override;
    bool ProcessSocketMessage(std::string message) override;
    void AddSelection(std::string selectionName);
    void InitializeData(EEPROM_Data data) override;
};

class Input_ColorPicker : public PageElement
{
private:
    uint8_t *m_dataRed, *m_dataGreen, *m_dataBlue;

public:
    Input_ColorPicker(std::string title, uint8_t *red, uint8_t *green, uint8_t *blue);

    void GetHTML(std::ostringstream &stream) override;
    void GetJavaCommandFunction(std::ostringstream &stream) override;
    void GetJavaOnMessageStatment(std::ostringstream &stream) override;
    std::string GetSocketMessage() override;
    bool ProcessSocketMessage(std::string message) override;
    void InitializeData(EEPROM_Data data) override;
};

class Input_Text : public PageElement
{
private:
    std::string *m_data;

public:
    Input_Text(std::string title, std::string *data);
    void GetHTML(std::ostringstream &stream) override;
    void GetJavaCommandFunction(std::ostringstream &stream) override;
    void GetJavaOnMessageStatment(std::ostringstream &stream) override;
    std::string GetSocketMessage() override;
    bool ProcessSocketMessage(std::string message) override;
    void InitializeData(EEPROM_Data data) override;
};

class Output_Text : public PageElement
{
private:
    std::string *m_data;

public:
    Output_Text(std::string title, std::string *data);
    void GetHTML(std::ostringstream &stream) override;
    void GetJavaCommandFunction(std::ostringstream &stream) override;
    void GetJavaOnMessageStatment(std::ostringstream &stream) override;
    std::string GetSocketMessage() override;
    bool ProcessSocketMessage(std::string message) override;
    void InitializeData(EEPROM_Data data) override;
};

#endif