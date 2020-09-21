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

//**************************_________ ToggleControl _________**************************

Control_Toggle::Control_Toggle(std::string title, uint8_t *controlVar)
    : PageElement{title}, m_data(controlVar)
{
}

void Control_Toggle::GetHTML(std::ostringstream &stream)
{
    stream << "<h1>"
           << m_title
           << "</h1>\n<label class=\"switch\">\n<input type=\"checkbox\" id=\""
           << m_ID
           << "\" onClick=\""
           << m_ID
           << "Clicked()\">\n<span class=\"slider round\"> </span> </label>\n";
}

void Control_Toggle::GetJavaCommandFunction(std::ostringstream &stream)
{
    stream << "function "
           << m_ID
           << "Clicked() { doSend(\""
           << m_ID
           << "\" + \"=\" + ~~(VAR_"
           << m_ID
           << ".checked)); }\n";
}
void Control_Toggle::GetJavaOnMessageStatment(std::ostringstream &stream)
{
    stream << "else if (evt.data.substring(0, 8) == \"" << m_ID << "\") { VAR_"
           << m_ID
           << ".checked = parseInt(evt.data.substr(9)); }\n";
}

std::string Control_Toggle::GetSocketMessage()
{
    std::ostringstream stream;
    stream << m_ID
           << "="
           << (int)*m_data;

    return stream.str();
}

bool Control_Toggle::ProcessSocketMessage(std::string message)
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

void Control_Toggle::InitializeData(EEPROM_Data data)
{
    m_EEPROM_Packtet = data;
    *m_data = data.data[0];
}

//**************************_________ SliderControl _________**************************

Control_Slider::Control_Slider(std::string title, uint8_t *controlVar, uint8_t max)
    : PageElement{title}, m_data(controlVar), m_max(max)
{
}

void Control_Slider::GetHTML(std::ostringstream &stream)
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

void Control_Slider::GetJavaCommandFunction(std::ostringstream &stream)
{
    stream << "function "
           << m_ID
           << "Changed() { doSend(\""
           << m_ID
           << "\" + \"=\" + VAR_"
           << m_ID
           << ".value); }\n";
}

void Control_Slider::GetJavaOnMessageStatment(std::ostringstream &stream)
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

std::string Control_Slider::GetSocketMessage()
{
    std::ostringstream stream;
    stream << m_ID
           << "="
           << (int)(*m_data);
    return stream.str();
}

bool Control_Slider::ProcessSocketMessage(std::string message)
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

void Control_Slider::InitializeData(EEPROM_Data data)
{
    m_EEPROM_Packtet = data;
    *m_data = data.data[0];
}

//**************************_________ DropDownControl _________**************************

Control_DropDown::Control_DropDown(std::string title, uint8_t *controlVar)
    : PageElement{title}, m_data(controlVar)
{
}

void Control_DropDown::GetHTML(std::ostringstream &stream)
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

void Control_DropDown::GetJavaCommandFunction(std::ostringstream &stream)
{
    stream << "function "
           << m_ID
           << "Changed() { doSend(\""
           << m_ID
           << "\" + \"=\" + VAR_"
           << m_ID
           << ".value); }\n"; // document.getElementById(\"" << m_ID << "_label\").innerHTML = \"" << m_title << " \" + Math.round(((" << m_ID << ".value / " << (int)m_max << ") * 100)) + \"%\";
}

void Control_DropDown::GetJavaOnMessageStatment(std::ostringstream &stream)
{
    stream << "else if (evt.data.substring(0, 8) == \""
           << m_ID
           << "\") { VAR_"
           << m_ID
           << ".value = parseInt(evt.data.substr(9)); }\n";
}

std::string Control_DropDown::GetSocketMessage()
{
    std::ostringstream stream;
    stream << m_ID << "=" << (int)(*m_data);
    return stream.str();
}

bool Control_DropDown::ProcessSocketMessage(std::string message)
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

void Control_DropDown::AddSelection(std::string selectionName)
{
    m_Selections.push_back(std::string(selectionName));
}

void Control_DropDown::InitializeData(EEPROM_Data data)
{
    m_EEPROM_Packtet = data;
    *m_data = data.data[0];
}

//**************************_________ ColorSelectControl _________**************************

Control_ColorPicker::Control_ColorPicker(std::string title, uint8_t *red, uint8_t *green, uint8_t *blue)
    : PageElement{title}, m_dataRed(red), m_dataGreen(green), m_dataBlue(blue)
{
}

void Control_ColorPicker::GetHTML(std::ostringstream &stream)
{
    //std::ostringstream stream;
    stream << "<h1>"
           << m_title
           << "</h1>\n<input type=\"color\" id=\""
           << m_ID
           << "\" value=\"#000000\" onChange=\""
           << m_ID
           << "Changed()\"/>\n";
    // return stream.str();
}

void Control_ColorPicker::GetJavaCommandFunction(std::ostringstream &stream)
{
    stream << "function "
           << m_ID
           << "Changed() { doSend(\""
           << m_ID
           << "\" + \"=\" + VAR_"
           << m_ID
           << ".value); }\n"; // document.getElementById(\"" << m_ID << "_label\").innerHTML = \"" << m_title << " \" + Math.round(((" << m_ID << ".value / " << (int)m_max << ") * 100)) + \"%\";
}

void Control_ColorPicker::GetJavaOnMessageStatment(std::ostringstream &stream)
{
    stream << "else if (evt.data.substring(0, 8) == \""
           << m_ID
           << "\") { VAR_"
           << m_ID
           << ".value = evt.data.substr(9); }\n";
}

std::string Control_ColorPicker::GetSocketMessage()
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

bool Control_ColorPicker::ProcessSocketMessage(std::string message)
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

void Control_ColorPicker::InitializeData(EEPROM_Data data)
{
    m_EEPROM_Packtet = data;
    *m_dataRed = data.data[0];
    *m_dataGreen = data.data[1];
    *m_dataBlue = data.data[2];
}

Display_Text::Display_Text(std::string title, std::string *data)
    : PageElement{title}, m_data(data)
{
}

void Display_Text::GetHTML(std::ostringstream &stream)
{
    stream << "<h1>"
           << m_title
           << "</h1>\n<h2 id=\""
           << m_ID
           << "\">"
           << *m_data
           << "</h2>\n";
}

void Display_Text::GetJavaCommandFunction(std::ostringstream &stream)
{
    stream << "\n";
}

void Display_Text::GetJavaOnMessageStatment(std::ostringstream &stream)
{
    stream << "else if (evt.data.substring(0, 8) == \""
           << m_ID
           << "\") { VAR_"
           << m_ID
           << ".innerHTML  = evt.data.substr(9); }\n";
}

std::string Display_Text::GetSocketMessage()
{
    std::ostringstream stream;
    stream << m_ID
           << "="
           << *m_data;
    return stream.str();
}

bool Display_Text::ProcessSocketMessage(std::string message)
{
    if (message.find(m_ID) != std::string::npos)
    {
        *m_data = message.erase(0, 9);
        return true;
    }
    return false;
}

void Display_Text::InitializeData(EEPROM_Data data) {}