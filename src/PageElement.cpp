#include "PageElement.h"

#include <string>
#include <vector>
#include <iostream>
#include <Arduino.h>
#include <sstream>

PageElement::PageElement(std::string title)
    : m_title(title)
{
}
PageElement::EEPROM_Data PageElement::GetEEPROMData()
{
    return m_EEPROM_Packtet;
}

//**************************_________ Toggle Input _________**************************

Input_Toggle::Input_Toggle(std::string title, uint8_t *controlVar)
    : PageElement{title}, m_data(controlVar)
{
}

void Input_Toggle::GetHTML(std::ostringstream &stream)
{
    stream << "<h1>"
           << m_title
           << "</h1>\n<label class=\"switch\">\n<input type=\"checkbox\" id=\""
           << m_ID
           << "\" onClick=\""
           << m_ID
           << "Clicked()\">\n<span class=\"slider round\"> </span> </label>\n";
}

void Input_Toggle::GetJavaCommandFunction(std::ostringstream &stream)
{
    stream << "function "
           << m_ID
           << "Clicked() { doSend(\""
           << m_ID
           << "\" + \"=\" + ~~(VAR_"
           << m_ID
           << ".checked)); }\n";
}
void Input_Toggle::GetJavaOnMessageStatment(std::ostringstream &stream)
{
    stream << "else if (evt.data.substring(0, 8) == \"" << m_ID << "\") { VAR_"
           << m_ID
           << ".checked = parseInt(evt.data.substr(9)); }\n";
}

std::string Input_Toggle::GetSocketMessage()
{
    std::ostringstream stream;
    stream << m_ID
           << "="
           << (int)*m_data;

    return stream.str();
}

bool Input_Toggle::ProcessSocketMessage(std::string message)
{
    if (message.find(m_ID) != std::string::npos)
    {
        std::string msgDat = message.erase(0, 9);

        *m_data = atoi(msgDat.c_str());
        m_EEPROM_Packtet.data[0] = *m_data;
        return true;
    }
    return false;
}

void Input_Toggle::InitializeData(EEPROM_Data data)
{
    m_EEPROM_Packtet = data;
    *m_data = data.data[0];
}

//**************************_________ Slider Input _________**************************

Input_Slider::Input_Slider(std::string title, uint8_t *controlVar, uint8_t max)
    : PageElement{title}, m_data(controlVar), m_max(max)
{
}

void Input_Slider::GetHTML(std::ostringstream &stream)
{
    stream << "<h1 id=\""
           << m_ID
           << "_label\">"
           << m_title
           << " 100%</h1>\n<input class=\"rangeSlider\" type=\"range\" id=\""
           << m_ID
           << "\" min=\"0\" max=\""
           << (int)m_max
           << "\" value=\"255\" onChange=\""
           << m_ID
           << "Changed()\">\n";
}

void Input_Slider::GetJavaCommandFunction(std::ostringstream &stream)
{
    stream << "function "
           << m_ID
           << "Changed() { doSend(\""
           << m_ID
           << "\" + \"=\" + VAR_"
           << m_ID
           << ".value); }\n";
}

void Input_Slider::GetJavaOnMessageStatment(std::ostringstream &stream)
{
    stream << "else if (evt.data.substring(0, 8) == \""
           << m_ID
           << "\") { VAR_"
           << m_ID
           << ".value = parseInt(evt.data.substr(9)); document.getElementById(\""
           << m_ID
           << "_label\").innerHTML = \""
           << m_title
           << " \" + Math.round(((VAR_"
           << m_ID
           << ".value / VAR_"
           << m_ID
           << ".max) * 100)) + \"%\"; }\n";
}

std::string Input_Slider::GetSocketMessage()
{
    std::ostringstream stream;
    stream << m_ID
           << "="
           << (int)(*m_data);
    return stream.str();
}

bool Input_Slider::ProcessSocketMessage(std::string message)
{
    if (message.find(m_ID) != std::string::npos)
    {
        std::string msgDat = message.erase(0, 9);
        *m_data = (static_cast<uint8_t>(atoi(msgDat.c_str())));
        m_EEPROM_Packtet.data[0] = *m_data;
        return true;
    }
    return false;
}

void Input_Slider::InitializeData(EEPROM_Data data)
{
    m_EEPROM_Packtet = data;
    *m_data = data.data[0];
}

//**************************_________ Dropdown Input _________**************************

Input_DropDown::Input_DropDown(std::string title, uint8_t *controlVar)
    : PageElement{title}, m_data(controlVar)
{
}

void Input_DropDown::GetHTML(std::ostringstream &stream)
{
    stream << "<h1>"
           << m_title
           << "</h1>\n<div class=\"box\">\n<select id=\""
           << m_ID
           << "\" onChange=\""
           << m_ID
           << "Changed()\">\n";

    for (int i = 0; i < m_Selections.size(); i++)
    {
        stream << "<option value=\""
               << i
               << "\">"
               << m_Selections[i]
               << "</option>\n";
    }

    stream << "</select>\n</div>\n";
}

void Input_DropDown::GetJavaCommandFunction(std::ostringstream &stream)
{
    stream << "function "
           << m_ID
           << "Changed() { doSend(\""
           << m_ID
           << "\" + \"=\" + VAR_"
           << m_ID
           << ".value); }\n";
}

void Input_DropDown::GetJavaOnMessageStatment(std::ostringstream &stream)
{
    stream << "else if (evt.data.substring(0, 8) == \""
           << m_ID
           << "\") { VAR_"
           << m_ID
           << ".value = parseInt(evt.data.substr(9)); }\n";
}

std::string Input_DropDown::GetSocketMessage()
{
    std::ostringstream stream;
    stream << m_ID
           << "="
           << (int)(*m_data);
    return stream.str();
}

bool Input_DropDown::ProcessSocketMessage(std::string message)
{
    if (message.find(m_ID) != std::string::npos)
    {
        std::string msgDat = message.erase(0, 9);

        *m_data = (static_cast<uint8_t>(atoi(msgDat.c_str())));
        m_EEPROM_Packtet.data[0] = *m_data;
        return true;
    }
    return false;
}

void Input_DropDown::AddSelection(std::string selectionName)
{
    m_Selections.push_back(std::string(selectionName));
}

void Input_DropDown::InitializeData(EEPROM_Data data)
{
    m_EEPROM_Packtet = data;
    *m_data = data.data[0];
}

//**************************_________ Color Picker Input _________**************************

Input_ColorPicker::Input_ColorPicker(std::string title, uint8_t *red, uint8_t *green, uint8_t *blue)
    : PageElement{title}, m_dataRed(red), m_dataGreen(green), m_dataBlue(blue)
{
}

void Input_ColorPicker::GetHTML(std::ostringstream &stream)
{
    stream << "<h1>"
           << m_title
           << "</h1>\n<input type=\"color\" id=\""
           << m_ID
           << "\" value=\"#000000\" onChange=\""
           << m_ID
           << "Changed()\"/>\n";
}

void Input_ColorPicker::GetJavaCommandFunction(std::ostringstream &stream)
{
    stream << "function "
           << m_ID
           << "Changed() { doSend(\""
           << m_ID
           << "\" + \"=\" + VAR_"
           << m_ID
           << ".value); }\n";
}

void Input_ColorPicker::GetJavaOnMessageStatment(std::ostringstream &stream)
{
    stream << "else if (evt.data.substring(0, 8) == \""
           << m_ID
           << "\") { VAR_"
           << m_ID
           << ".value = evt.data.substr(9); }\n";
}

std::string Input_ColorPicker::GetSocketMessage()
{
    std::ostringstream stream;

    char colorHex[7];
    unsigned int r, g, b;
    r = *m_dataRed;
    g = *m_dataGreen;
    b = *m_dataBlue;
    sprintf(colorHex, "%02x%02x%02x", r, g, b);
    colorHex[6] = '\0';
    stream << m_ID
           << "=#"
           << colorHex;
    return stream.str();
}

bool Input_ColorPicker::ProcessSocketMessage(std::string message)
{
    if (message.find(m_ID) != std::string::npos)
    {
        std::string msgDat = message.erase(0, 10);
        const char *str = msgDat.c_str();
        unsigned int r, g, b;
        sscanf(str, "%02x%02x%02x", &r, &g, &b);

        *m_dataRed = (uint8_t)r;
        *m_dataGreen = (uint8_t)g;
        *m_dataBlue = (uint8_t)b;

        m_EEPROM_Packtet.data[0] = (uint8_t)r;
        m_EEPROM_Packtet.data[1] = (uint8_t)g;
        m_EEPROM_Packtet.data[2] = (uint8_t)b;

        return true;
    }
    return false;
}

void Input_ColorPicker::InitializeData(EEPROM_Data data)
{
    m_EEPROM_Packtet = data;
    *m_dataRed = data.data[0];
    *m_dataGreen = data.data[1];
    *m_dataBlue = data.data[2];
}

//**************************_________ Text Input _________**************************

Input_Text::Input_Text(std::string title, std::string *data)
    : PageElement{title}, m_data(data)
{
}

void Input_Text::GetHTML(std::ostringstream &stream)
{
    stream << "<h1>"
           << m_title
           << "</h1>\n<textarea id=\""
           << m_ID
           << "\" rows=\"4\" style=\"font-size: 18pt\" onChange=\""
           << m_ID
           << "Changed()\"> </textarea>\n";
}

void Input_Text::GetJavaCommandFunction(std::ostringstream &stream)
{
    stream << "function "
           << m_ID
           << "Changed() { doSend(\""
           << m_ID
           << "\" + \"=\" + VAR_"
           << m_ID
           << ".value); }\n";
}

void Input_Text::GetJavaOnMessageStatment(std::ostringstream &stream)
{
    stream << "else if (evt.data.substring(0, 8) == \""
           << m_ID
           << "\") { VAR_"
           << m_ID
           << ".value = evt.data.substr(9); }\n";
}

std::string Input_Text::GetSocketMessage()
{
    std::ostringstream stream;
    stream << m_ID
           << "="
           << *m_data;
    return stream.str();
}

bool Input_Text::ProcessSocketMessage(std::string message)
{
    if (message.find(m_ID) != std::string::npos)
    {
        *m_data = message.erase(0, 9);
        return true;
    }
    return false;
}

void Input_Text::InitializeData(EEPROM_Data data) {}

//**************************_________ Text Output _________**************************

Output_Text::Output_Text(std::string title, std::string *data)
    : PageElement{title}, m_data(data)
{
}

void Output_Text::GetHTML(std::ostringstream &stream)
{
    stream << "<h1>"
           << m_title
           << "</h1>\n<h2 id=\""
           << m_ID
           << "\">"
           << *m_data
           << "</h2>\n";
}

void Output_Text::GetJavaCommandFunction(std::ostringstream &stream)
{
    stream << "\n";
}

void Output_Text::GetJavaOnMessageStatment(std::ostringstream &stream)
{
    stream << "else if (evt.data.substring(0, 8) == \""
           << m_ID
           << "\") { VAR_"
           << m_ID
           << ".innerHTML  = evt.data.substr(9); }\n";
}

std::string Output_Text::GetSocketMessage()
{
    std::ostringstream stream;
    stream << m_ID
           << "="
           << *m_data;
    return stream.str();
}

bool Output_Text::ProcessSocketMessage(std::string message)
{
    if (message.find(m_ID) != std::string::npos)
    {
        *m_data = message.erase(0, 9);
        return true;
    }
    return false;
}

void Output_Text::InitializeData(EEPROM_Data data) {}