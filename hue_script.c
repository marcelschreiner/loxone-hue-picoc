//       ___ _    _ _ _           _  _          
//      | _ \ |_ (_) (_)_ __ ___ | || |_  _ ___ 
//      |  _/ ' \| | | | '_ (_-< | __ | || / -_)
//      |_| |_||_|_|_|_| .__/__/ |_||_|\_,_\___|
//                     |_|                      
//       PicoC integration for Loxone Minisever 
// ***************************************************
//  Please add your configuration in the highlighted
//  section below. DO NOT CHANGE ANYTHING ELSE!
// ***************************************************

#define ENABLE_DEBUGGING    (0)
#define RX_BUFFER_SIZE      (40000)
#define RX_BLOCK_SIZE       (128)

enum bulb_t__
{
    TYPE_SINGLE_RGB,
    TYPE_SINGLE_TUNABLE,
    TYPE_SINGLE_DIM,
    TYPE_SINGLE_ONOFF,
    TYPE_GROUP_RGB,
    TYPE_GROUP_TUNABLE,
    TYPE_GROUP_DIM,
    TYPE_GROUP_ONOFF,
    TYPE_NONE
};

int bulbType[13];
int bulbId[13];

// ***************************************************
//  Configure your system here                   START
// ***************************************************
char *IP_ADDRESS = "192.168.1.10";
char *PORT       = "80";
char *USERNAME   = "yourphilipshueapikey";

// Set type of bulb
bulbType[0]  = TYPE_SINGLE_TUNABLE;
bulbType[1]  = TYPE_SINGLE_TUNABLE;
bulbType[2]  = TYPE_SINGLE_TUNABLE;
bulbType[3]  = TYPE_NONE;
bulbType[4]  = TYPE_NONE;
bulbType[5]  = TYPE_SINGLE_RGB;
bulbType[6]  = TYPE_SINGLE_RGB;
bulbType[7]  = TYPE_NONE;
bulbType[8]  = TYPE_NONE;
bulbType[9]  = TYPE_NONE;
bulbType[10] = TYPE_SINGLE_TUNABLE;
bulbType[11] = TYPE_SINGLE_TUNABLE;
bulbType[12] = TYPE_SINGLE_RGB;

// Set bulb or group id
bulbId[0] = 14;
bulbId[1] = 15;
bulbId[2] = 16;
bulbId[3] = 17;
bulbId[4] = 18;
bulbId[5] = 19;
bulbId[6] = 20;
bulbId[7] = 21;
bulbId[8] = 22;
bulbId[9] = 23;
bulbId[10] = 25;
bulbId[11] = 26;
bulbId[12] = 13;

// ***************************************************
//  Configure your system here                     END
// ***************************************************

int IsGroup(int type)
{
    int isGroup = 0;
    switch(type)
    {
        case TYPE_SINGLE_RGB:
        case TYPE_SINGLE_TUNABLE:
        case TYPE_SINGLE_DIM:
        case TYPE_SINGLE_ONOFF:
            isGroup = 0;
            break;

        case TYPE_GROUP_RGB:
        case TYPE_GROUP_TUNABLE:
        case TYPE_GROUP_DIM:
        case TYPE_GROUP_ONOFF:
            isGroup = 1;
            break;
    }
    return isGroup;
}


void SendCommand(int isGroup, int id, char *command)
{
    char streamname[100];
    char selector[30];
    char txBuffer[1024];
    char rxBuffer[RX_BUFFER_SIZE];
    char szTmpBuffer[RX_BLOCK_SIZE];
    int nCnt;
    int nBytesReceived = 0;

    // Create stream
    sprintf(streamname, "/dev/tcp/%s/%s/", IP_ADDRESS, PORT);
    STREAM *TcpStream = stream_create(streamname, 0, 0);
    if (TcpStream == NULL)
    {
        if (ENABLE_DEBUGGING)
        {
            printf("Creating Stream failed");
        }
        stream_close(TcpStream);
        return;
    }

    if (0 == isGroup)
    {
        sprintf(selector, "lights/%d/state", id);
    }
    else
    {
        sprintf(selector, "groups/%d/action", id);
    }
    
    sprintf(txBuffer, "PUT /api/%s/%s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\nContent-Length: %d\r\n\r\n%s", USERNAME, selector, IP_ADDRESS, strlen(command), command);
    if (ENABLE_DEBUGGING)
    {
        printf(txBuffer);
    }
    stream_write(TcpStream, txBuffer, sizeof(txBuffer));
    stream_flush(TcpStream);
    
    // read stream
    if (ENABLE_DEBUGGING)
    {
        do
        {
            nCnt = stream_read(TcpStream, szTmpBuffer, RX_BLOCK_SIZE, 4000);
            if (nCnt + nBytesReceived > RX_BUFFER_SIZE)
            {
                nBytesReceived = -1;
                break; //File is too large
            }
            else if (nCnt > 0)
            {
                if (ENABLE_DEBUGGING)
                {
                    strncpy((char *)rxBuffer + nBytesReceived, szTmpBuffer, nCnt);
                }
                nBytesReceived += nCnt;
            }
        } while (nCnt > 0);
    
        printf(strstr(rxBuffer, "[{"));
    }
    stream_close(TcpStream);
}


void SetOnOff(int id, int state, int isGroup)
{
    char command[50];

    if (0 == state)
    {
        sprintf(command, "{\"on\": false, \"transitiontime\": 10}");
    }
    else
    {
        sprintf(command, "{\"on\": true, \"transitiontime\": 10}");
    }
    SendCommand(isGroup, id, command);
}


void SetDim(int id, int brightness, int isGroup)
{
    char command[60];

    // Check if brightness is set to 0 and leave early
    if (0 == brightness)
    {
        SetOnOff(id, 0, isGroup);
    }
    else
    {
        // Change range of brightness 1-100 -> 1-255
        brightness = (brightness * 255) / 100;

        sprintf(command, "{\"on\": true, \"bri\": %d, \"transitiontime\": 10}", brightness);
        SendCommand(isGroup, id, command);
    }
}


void SetTunable(int id, int value, int isGroup)
{
    char command[100];
    int brightness;
    int temperature;

    brightness  = (value - 200000000) / 10000;                  // 0-100
    temperature = (value - 200000000) - (brightness * 10000);   // Kelvin 2700 - 6500

    brightness  = (brightness * 255) / 100;                     // 0-255
    temperature = 1000000 / temperature;                        // 154 - 370

    // Check if input value was set to 0 or brightness is set to 0
    if ((0 == value)||(0 == brightness))
    {
        SetOnOff(id, 0, isGroup);
    }
    else
    {
        sprintf(command, "{\"on\": true, \"bri\": %d, \"ct\": %d, \"transitiontime\": 10}", brightness, temperature);
        SendCommand(isGroup, id, command);
    }
}


void SetColorXYB(int id, float red, float green, float blue, int isGroup)
{
    float cx, cy, bri;
    float X, Y, Z;
    char command[100];

    bri = blue;
    if (bri < green)
    {
        bri = green;
    }
    if (bri < red)
    {
        bri = red;
    }
    bri = bri * 2.55;

    blue = blue / 100;
    green = green / 100;
    red = red / 100;

    // Apply gamma correction
    if (red > 0.04045)
    {
        red = pow((red + 0.055) / 1.055, 2.4);
    }
    else
    {
        red = red / 12.92;
    }
    if (green > 0.04045)
    {
        green = pow((green + 0.055) / 1.055, 2.4);
    }
    else
    {
        green = green / 12.92;
    }
    if (blue > 0.04045)
    {
        blue = pow((blue + 0.055) / 1.055, 2.4);
    }
    else
    {
        blue = blue / 12.92;
    }

    // Wide gamut conversion D65
    X = red * 0.664511f + green * 0.154324f + blue * 0.162028f;
    Y = red * 0.283881f + green * 0.668433f + blue * 0.047685f;
    Z = red * 0.000088f + green * 0.072310f + blue * 0.986039f;

    // Calculate xy and bri
    if ((X + Y + Z) == 0)
    {
        cx = 0;
        cy = 0;
    }
    else
    { // round to 4 decimal max (=api max size)
        cx = X / (X + Y + Z);
        cy = Y / (X + Y + Z);
    }

    sprintf(command, "{\"xy\": [%f,%f],\"bri\": %d,\"on\":true, \"transitiontime\": 10}", cx, cy, bri);

    SendCommand(isGroup, id, command);
}


void SetColorHSB(int id, float red, float green, float blue, int isGroup)
{
    float hue, sat, bri;
    char command[100];

    hue = 0;
    sat = 0;
    bri = 0;

    if (blue > 0 || green > 0 || red > 0)
    {
        if ((red >= green) && (green >= blue))
        {
            if (red == blue)
            {
                hue = 0;
            }
            else
            {
                hue = 60 * (green - blue) / (red - blue);
            }
            sat = (red - blue) / red;
            bri = red;
        }
        else if ((green > red) && (red >= blue))
        {
            hue = 60 * (2 - (red - blue) / (green - blue));
            sat = (green - blue) / green;
            bri = green;
        }
        else if ((green >= blue) && (blue > red))
        {
            hue = 60 * (2 + (blue - red) / (green - red));
            sat = (green - red) / green;
            bri = green;
        }
        else if ((blue > green) && (green > red))
        {
            hue = 60 * (4 - (green - red) / (blue - red));
            sat = (blue - red) / blue;
            bri = blue;
        }
        else if ((blue > red) && (red >= green))
        {
            hue = 60 * (4 + (red - green) / (blue - green));
            sat = (blue - green) / blue;
            bri = blue;
        }
        else if ((red >= blue) && (blue > green))
        {
            hue = 60 * (6 - (blue - green) / (red - green));
            sat = (red - green) / red;
            bri = red;
        }

        // hue = 0-65535, sat 0-255, bri 0-255
        hue = hue / 360 * 65535;
        sat = sat * 255;
        bri = bri * 2.55;
    }

    sprintf(command, "{\"bri\": %d, \"hue\": %d, \"sat\": %d, \"on\": true, \"transitiontime\": 10}", (int)bri, (int)hue, (int)sat);

    SendCommand(isGroup, id, command);
}


void SetRGB(int id, int value, int isGroup)
{
    int red;
    int green;
    int blue;

    // Check if brightness is set to 0 and leave early
    if (0 == value)
    {
        SetOnOff(id, 0, isGroup);
        return;
    }

    blue  = value / 1000000;
    green = (value - (blue * 1000000)) / 1000;
    red   = value - (blue * 1000000) - (green * 1000);

    // Use ether HSB or XYB to set RGB colors (Choose whatever works best for you!)
    SetColorXYB(id, (float)red, (float)green, (float)blue, isGroup);
    //SetColorHSB(id, (float)red, (float)green, (float)blue, isGroup);
}


void UpdateLamp(int i, int value)
{
    int id = bulbId[i];
    switch (bulbType[i])
    {
        case TYPE_SINGLE_RGB:
        case TYPE_GROUP_RGB:
            if (value < 200000000)
            {
                SetRGB(id, value, IsGroup(bulbType[i]));
            }
            else
            {
                SetTunable(id, value, IsGroup(bulbType[i]));
            }
            break;

        case TYPE_SINGLE_TUNABLE:
        case TYPE_GROUP_TUNABLE:
            if ((200000000 <= value) || (0 == value))
            {
                SetTunable(id, value, IsGroup(bulbType[i]));
            }
            break;

        case TYPE_SINGLE_DIM:
        case TYPE_GROUP_DIM:
            SetDim(id, value, IsGroup(bulbType[i]));
            break;

        case TYPE_SINGLE_ONOFF:
        case TYPE_GROUP_ONOFF:
            SetOnOff(id, value, IsGroup(bulbType[i]));
            break;
    }
}


// Main application
int inputsThatChanged;
int i;
int refreshCounter = 0;

while (1)
{
    // Get a bitmask of the changed inputs (bit 0 = first input of object, starts with text inputs followed by analog inputs).
    inputsThatChanged = getinputevent();

    // First fast round to update all lights that changed as quickly as possible
    for (i = 0; i < 13; i++)
    {
        // Check if input has changed
        if (inputsThatChanged & 0x8 << i)
        {
            UpdateLamp(i, (int)getinput(i));
        }
    }

    // Second slower round (with sleep) to update all lights that changed did not get updated in the first round (e.g. due to network congestion)
    for (i = 0; i < 13; i++)
    {
        // Check if input has changed
        if (inputsThatChanged & 0x8 << i)
        {
            UpdateLamp(i, (int)getinput(i));
            sleep(10);
        }
    }

    refreshCounter = refreshCounter + 1;

    // Refresh all lights approximately every 60s (to keep hue lights in sync with Loxone Miniserver, if they are changed via the Hue app)
    if (600 < refreshCounter)
    {
        refreshCounter = 0;
        for (i = 0; i < 13; i++)
        {
            UpdateLamp(i, (int)getinput(i));
            sleep(10);
        }
    }
    sleep(100);
}
