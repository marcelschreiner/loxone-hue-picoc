#define BUFF_SIZE (40000)
#define RD_BLOCK_SIZE (128)
#define DEBUG_LEVEL (0)
#define DIMMER_MIN (15)
#define DIMMER_MAX (100)
// Transition time in [0.1s]
#define TRANSITION_TIME (10)

char *IP_ADDRESS = "192.168.1.10";
char *PORT = "80";
char *USERNAME = "yourphilipshueapikey";

enum lightOrGroup_t__
{
    LIGHT,
    GROUP
};

enum inputType_t__
{
    TYPE_SINGLE_RGB_HSB, // RGB-Eingang (Eingang LoxoneFormat, 9-stellige Zahl die RGB codiert mit 100100100 für weiﬂ. Ansteuerung der Lampe via Hue/Sat/Bri
    TYPE_SINGLE_RBG_XYB, // RGB-Eingang (wie RGB_HSB, aber ansteuerung der Lampe via X/Y/Bri)
    TYPE_SINGLE_DIM,     // Dimmereingang. Eingangswert muss im Bereich DIMMER_MIN / DIMMER_MAX sein. Ansteuerung der Lampe via Bri.
    TYPE_SINGLE_TUNABLE, // Tunable white (via Bri/Ct)
    TYPE_SINGLE_ONOFF,   // ON/OF - Eingang (z.B. für Steckdosen-Adapter)
    TYPE_GROUP_RGB_HSB,  // RGB-Eingang (wie '0', aber steuert Lampengruppe via Hue/Sat/Bri)
    TYPE_GROUP_RGB_XYB,  // RGB-Eingang (wie '0', aber steuert Lampengruppe via X/Y/Bri)
    TYPE_GROUP_DIM,      // Dimmereingang, wie 1, aber f¸r Gruppe oder Szene fuer diese Gruppe eine Szene angegeben, so wird die Szene bei Wert 1 oder 100 gesetzt
    TYPE_GROUP_TUNABLE,  // Tunable white (via Bri/Ct)
    TYPE_NONE
};

int inputType[13];
inputType[0] = TYPE_SINGLE_TUNABLE;  // Decke Türe (Bad Kinder)
inputType[1] = TYPE_SINGLE_TUNABLE;  // Decke Dusche (Bad Kinder)
inputType[2] = TYPE_SINGLE_TUNABLE;  // Decke Mitte (Bad Kinder)
inputType[3] = TYPE_NONE;
inputType[4] = TYPE_NONE;
inputType[5] = TYPE_SINGLE_RGB_HSB;  // Spiegel links (Bad Kinder)
inputType[6] = TYPE_SINGLE_RGB_HSB;  // Spiegel rechts (Bad Kinder)
inputType[7] = TYPE_NONE;
inputType[8] = TYPE_NONE;
inputType[9] = TYPE_NONE;
inputType[10] = TYPE_SINGLE_TUNABLE;  // Blop Bett
inputType[11] = TYPE_SINGLE_TUNABLE;  // Blop Fenster
inputType[12] = TYPE_SINGLE_RGB_HSB;  // Ambiente (Bad Kinder)

int lightOrGroupID[13];

// Zuweisung der Lampen- oder GruppenIDs.
// Definiert welcher Eingang des Bausteins welche Lampe bzw. Gruppe ansteuert:
lightOrGroupID[0] = 14;
lightOrGroupID[1] = 15;
lightOrGroupID[2] = 16;
lightOrGroupID[3] = 17;
lightOrGroupID[4] = 18;
lightOrGroupID[5] = 19;
lightOrGroupID[6] = 20;
lightOrGroupID[7] = 21;
lightOrGroupID[8] = 22;
lightOrGroupID[9] = 23;
lightOrGroupID[10] = 25;
lightOrGroupID[11] = 26;
lightOrGroupID[12] = 13;

// Ende der Konfiguration...

char streamname[100];
sprintf(streamname, "/dev/tcp/%s/%s/", IP_ADDRESS, PORT);

void updateLamp(int idx, int value)
{
    switch (inputType[idx])
    {
        case TYPE_SINGLE_RGB_HSB:
            if (value < 200000000)
            {
                // RGB
                setColorHSB(lightOrGroupID[idx], value, LIGHT);
            }
            else
            {
                // Tunable white
                setCtBri(lightOrGroupID[idx], value, LIGHT);
            }
            break;

        case TYPE_SINGLE_RBG_XYB:
            if (value < 200000000)
            {
                // RGB
                setColorXYB(lightOrGroupID[idx], value, LIGHT);
            }
            else
            {
                // Tunable white
                setCtBri(lightOrGroupID[idx], value, LIGHT);
            }
            break;

        case TYPE_SINGLE_DIM:
            setBrightness(lightOrGroupID[idx], value, LIGHT, 0);
            break;

        case TYPE_GROUP_DIM:
            setBrightness(lightOrGroupID[idx], value, GROUP, idx);
            break;

        case TYPE_SINGLE_ONOFF:
            setOnOff(lightOrGroupID[idx], value);
            break;

        case TYPE_GROUP_RGB_HSB:
            if (value < 200000000)
            {
                // RBG
                setColorHSB(lightOrGroupID[idx], value, GROUP);
            }
            else
            {
                // Tunable white
                setCtBri(lightOrGroupID[idx], value, GROUP);
            }
            break;

        case TYPE_GROUP_RGB_XYB:
            if (value < 200000000)
            {
                // RGB
                setColorXYB(lightOrGroupID[idx], value, GROUP);
            }
            else
            {
                // Tunable white
                setCtBri(lightOrGroupID[idx], value, GROUP);
            }
            break;

        case TYPE_SINGLE_TUNABLE:
            if ((value >= 200000000) || (value == 0))
            {
                // Ignore RGB only allow Tunable white
                setCtBri(lightOrGroupID[idx], value, LIGHT);
            }
            break;

        case TYPE_GROUP_TUNABLE:
            if ((value >= 200000000) || (value == 0))
            {
                // Ignore RGB only allow Tunable white
                setCtBri(lightOrGroupID[idx], value, GROUP);
            }
            break;

        default:
            printf("HUE Script: Invalid type configured");
            break;
    }
}

void setBrightness(int lightID, int bri, int type, int idx)
{
    char command[200];
    char cmdscene[100];
    char selector[20];

    // Normieren von 35-100 -> 1-255
    if (bri > 0)
    {
        bri = (bri - DIMMER_MIN) / (DIMMER_MAX - DIMMER_MIN) * 254 + 1;
    }

    switch (type)
    {
    case LIGHT:
        sprintf(selector, "lights/%d/state", lightID);
        break;

    case GROUP:
        sprintf(selector, "groups/%d/action", lightID);
        break;
    }

    if (bri == 0)
    {
        sprintf(command, "{\"on\": false}");
        if (DEBUG_LEVEL > 0)
        {
            printf("Light %d OFF", lightID);
        }
    }
    else
    {
        sprintf(command, "{\"on\": true, \"bri\": %d, \"transitiontime\": %d}", (int)(bri), TRANSITION_TIME);
        if (DEBUG_LEVEL > 0)
        {
            printf("Light %d ON %d%%", lightID, (int)((bri - 1) / 2.55) + 1);
        }
    }
    sendCommand(selector, command);
}


void setCtBri(int lightID, float ctbrivalue, int type)
{
    char command[100];
    char selector[20];
    float bri, ct;
    int briNorm, miredNorm;

    bri = floor((ctbrivalue - 200000000) / 10000);        // 0-100
    ct = floor((ctbrivalue - 200000000) - (bri * 10000)); // Wert in Kelvin, von 2700 - 6500

    briNorm = (int)round(bri * 2.55);     // 0-255
    miredNorm = (int)round(1000000 / ct); // Wert von 154 - 370

    if (type == LIGHT)
    {
        sprintf(selector, "lights/%d/state", lightID);
    }
    else if (type == GROUP)
    {
        sprintf(selector, "groups/%d/action", lightID);
    }

    if (bri == 0)
    {
        sprintf(command, "{\"on\": false}");
        if (DEBUG_LEVEL > 0)
            printf("Light %d OFF", lightID);
    }
    else
    {
        sprintf(command, "{\"on\": true, \"bri\": %d, \"ct\": %d, \"transitiontime\": %d}", briNorm, miredNorm, TRANSITION_TIME);
        if (DEBUG_LEVEL > 0)
            printf("Light %d ON %d%% %dK", lightID, (int)bri, (int)ct);
    }

    sendCommand(selector, command);
}

void setOnOff(int lightID, float bri)
{
    char command[100];
    char selector[20];

    sprintf(selector, "lights/%d/state", lightID);

    if (bri == 0)
    {
        sprintf(command, "{\"on\": false}");
        if (DEBUG_LEVEL > 0)
            printf("Light %d OFF", lightID);
    }
    else
    {
        sprintf(command, "{\"on\": true}");
        if (DEBUG_LEVEL > 0)
            printf("Light %d ON", lightID);
    }

    sendCommand(selector, command);
}

void setColorXYB(int lightOrGroupID, float rgbvalue, int type)
{
    char buffer[256];
    float red, green, blue;
    float cx, cy, bri;
    float X, Y, Z;
    char command[100];
    char selector[50];

    blue = floor(rgbvalue / 1000000);
    green = floor((rgbvalue - blue * 1000000) / 1000);
    red = rgbvalue - blue * 1000000 - green * 1000;

    bri = blue;
    if (bri < green)
        bri = green;
    if (bri < red)
        bri = red;
    bri = bri * 2.55;

    blue = blue / 100;
    green = green / 100;
    red = red / 100;

    // // Apply gamma correction v1
    // if (red > 0.04055)
    // {
    //     red = pow((red + 0.055) / 1.055, 2.4);
    // }
    // else
    // {
    //     red = red / 12.92;
    // }
    // if (green > 0.04055)
    // {
    //     green = pow((green + 0.055) / 1.055, 2.4);
    // }
    // else
    // {
    //     green = green / 12.92;
    // }
    // if (blue > 0.04055)
    // {
    //     blue = pow((blue + 0.055) / 1.055, 2.4);
    // }
    // else
    // {
    //     blue = blue / 12.92;
    // }
    // // Convert to XYZ
    // X = red * 0.649926 + green * 0.103455 + blue * 0.197109;
    // Y = red * 0.234327 + green * 0.743075 + blue * 0.022598;
    // Z = red * 0.013932 + green * 0.053077 + blue * 1.035763;

    // Apply gamma correction v2
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

    if (type == LIGHT)
    {
        sprintf(selector, "lights/%d/state", lightOrGroupID);
    }
    else if (type == GROUP)
    {
        sprintf(selector, "groups/%d/action", lightOrGroupID);
    }

    if (bri == 0)
    {
        sprintf(command, "{\"on\": false}");
    }
    else
    {
        sprintf(command, "{\"xy\": [%f,%f],\"bri\": %d,\"on\":true, \"transitiontime\": %d}", cx, cy, bri, TRANSITION_TIME);
    }

    if (DEBUG_LEVEL > 1)
        printf(command);

    sendCommand(selector, command);
}

void setColorHSB(int lightOrGroupID, float rgbvalue, int type)
{
    char buffer[256];
    float red, green, blue;
    float hue, sat, bri;
    char command[100];
    char selector[50];

    // Hinweis: rgbvalue ist red + green*1000 + blue*1000000
    blue = floor(rgbvalue / 1000000);
    green = floor((rgbvalue - blue * 1000000) / 1000);
    red = rgbvalue - blue * 1000000 - green * 1000;

    // nochmal umrechnen nach hue irgendwie, weil die Living Colors Gen2 irgendwie nich gehen mit xy
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

        // Werte für HUE normieren (hue = 0-65535, sat 0-255, bri 0-255)
        hue = hue / 360 * 65535;
        sat = sat * 255;
        bri = bri * 2.55;
    }

    // Ausgeben ins Log
    if (DEBUG_LEVEL > 1)
        printf("value:%09d, b:%d, g:%d, r: %d, hue:%d, sat: %d, bri: %d\n", rgbvalue, blue, green, red, (int)hue, (int)sat, (int)bri);

    if (bri == 0)
    {
        sprintf(command, "{\"on\": false}");
        if (DEBUG_LEVEL > 0 && type == LIGHT)
            printf("Light %d OFF", lightOrGroupID);
        if (DEBUG_LEVEL > 0 && type == GROUP)
            printf("Group %d OFF", lightOrGroupID);
    }
    else
    {
        sprintf(command, "{\"bri\": %d, \"hue\": %d, \"sat\": %d, \"on\": true, \"transitiontime\": %d}", (int)bri, (int)hue, (int)sat, TRANSITION_TIME);
        if (DEBUG_LEVEL > 0 && type == LIGHT)
            printf("Light %d ON %d%%, %d∞ %d%%", lightOrGroupID, (int)((bri - 1) / 2.55) + 1, (int)(hue / 65535 * 360), (int)(sat / 2.55));
        if (DEBUG_LEVEL > 0 && type == GROUP)
            printf("Group %d ON %d%%, %d∞ %d%%", lightOrGroupID, (int)((bri - 1) / 2.55) + 1, (int)(hue / 65535 * 360), (int)(sat / 2.55));
    }

    if (type == LIGHT)
    {
        sprintf(selector, "lights/%d/state", lightOrGroupID);
    }
    else if (type == GROUP)
    {
        sprintf(selector, "groups/%d/action", lightOrGroupID);
    }

    sendCommand(selector, command);
}

void sendCommand(char *selector, char *command)
{

    if (DEBUG_LEVEL > 1)
        char szBuffer[BUFF_SIZE];
    char szTmpBuffer[RD_BLOCK_SIZE];
    int nCnt;
    int nBytesReceived = 0;
    int blData = 0;

    STREAM *TcpStream = stream_create(streamname, 0, 0);
    if (TcpStream == NULL)
    {
        printf("Creating Stream failed");
        stream_close(TcpStream);
        return;
    }

    char buffer[1024];
    sprintf(buffer, "PUT /api/%s/%s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\nContent-Length: %d\r\n\r\n%s",
            USERNAME, selector, IP_ADDRESS, strlen(command), command);
    if (DEBUG_LEVEL > 1)
        printf(buffer);
    stream_write(TcpStream, buffer, sizeof(buffer));
    stream_flush(TcpStream);
    // read stream
    do
    {
        nCnt = stream_read(TcpStream, szTmpBuffer, RD_BLOCK_SIZE, 4000);
        if (nCnt + nBytesReceived > BUFF_SIZE)
        {
            nBytesReceived = -1;
            break; //File is too large
        }
        else if (nCnt > 0)
        {
            if (DEBUG_LEVEL > 1)
                strncpy((char *)szBuffer + nBytesReceived, szTmpBuffer, nCnt);
            nBytesReceived += nCnt;
        }
    } while (nCnt > 0);
    if (DEBUG_LEVEL > 1)
        printf(strstr(szBuffer, "[{"));
    //sleep(150);
    stream_close(TcpStream);
}

// Main application
int inputsThatChanged;
int i;

while (1)
{
    // Get a bitmask which contains the changes of inputs (bit 0 = first input of object, starts with text inputs followed by analog inputs).
    inputsThatChanged = getinputevent();

    // Loop from AI1 to AI13 and update lamp if the coresponding input changed
    for (i = 0; i < 13; i++)
    {
        if (inputsThatChanged & 0x8 << i)
        {
            updateLamp(i, (int)getinput(i));
            //sleep(10);
        }
    }

    // Loop from AI1 to AI13 and update lamp if the coresponding input changed
    for (i = 0; i < 13; i++)
    {
        if (inputsThatChanged & 0x8 << i)
        {
            updateLamp(i, (int)getinput(i));
            sleep(100);
        }
    }

    sleep(100);
}
