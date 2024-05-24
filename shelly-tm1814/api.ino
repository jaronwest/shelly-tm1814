// PUT endpoints
void putPowerOn() {
  power = 1;
  server.send(200, "text/json", "{\"status\": \"success\"}");
  handleStrip();
}

void putPowerOff() {
  power = 0;
  server.send(200, "text/json", "{\"status\": \"success\"}");
  handleStrip();
}

void putRainbow() {
  String put_body = server.arg("plain");
  JSONVar bodyObject = JSON.parse(put_body);
  rainbowSpeed = (int) bodyObject["speed"];
  rainbowBrightness = (int) bodyObject["brightness"];
  ww = (int) bodyObject["ww"];
  rainbowSize = (double) bodyObject["size"];

  mode = 2;

  server.send(200, "text/json", "{\"status\": \"success\"}");
  handleStrip();
}

void putColor() {
  String put_body = server.arg("plain");
  JSONVar bodyObject = JSON.parse(put_body);
  ww = (int) bodyObject["ww"];
  r = (int) bodyObject["red"];
  g = (int) bodyObject["green"];
  b = (int) bodyObject["blue"];

  mode = 1;

  server.send(200, "text/json", "{\"status\": \"success\"}");
  handleStrip();
}

void putGradient() {
  String put_body = server.arg("plain");
  JSONVar bodyObject = JSON.parse(put_body);

/*##
  use an array of colors with 1, 2, or more colors
  also include an optional point change, otherwise assumed even distribution
  0-100 as a percent of the whole strip
  [0, 50, 100] would be three even colors
  [0, 25, 100] would switch from color 1 to 2 in only 25% of the strip then 75% to color 3
*/
//## also generate an array of deltas to work with
  ww = (int) bodyObject["ww"];
  r = (int) bodyObject["red"];
  g = (int) bodyObject["green"];
  b = (int) bodyObject["blue"];

  ww2 = (int) bodyObject["ww2"];
  r2 = (int) bodyObject["red2"];
  g2 = (int) bodyObject["green2"];
  b2 = (int) bodyObject["blue2"];
  
  wwDelta = (float) (ww2 - ww) / pixelCount;
  rDelta = (float) (r2 - r) / pixelCount;
  gDelta = (float) (g2 - g) / pixelCount;
  bDelta = (float) (b2 - b) / pixelCount;

  mode = 3;

  server.send(200, "text/json", "{\"status\": \"success\"}");
  handleStrip();
}

// GET endpoints
void getPower() {
  String powerStr = power ? "on" : "off";
  server.send(200, "text/json", "{\"power\": \"" + powerStr + "\"}");
}

void getRainbow() {
  String rainbowJSON = String("{") + "\"speed\": " + rainbowSpeed;
  rainbowJSON = rainbowJSON + ", \"size\": " + rainbowSize;
  rainbowJSON = rainbowJSON + ", \"brightness\": " + rainbowBrightness;
  rainbowJSON = rainbowJSON + ", \"ww\": " + ww + "}";
  server.send(200, "text/json", rainbowJSON);
}

/*##
  use an array of colors with 1, 2, or more colors
  also include an optional point change, otherwise assumed even distribution
  0-100 as a percent of the whole strip
  [0, 50, 100] would be three even colors
  [0, 25, 100] would switch from color 1 to 2 in only 25% of the strip then 75% to color 3
*/
void getColor() {
  String colorJSON = String("{[{") + "\"ww\": " + ww;
  colorJSON = colorJSON + ", \"red\": " + r;
  colorJSON = colorJSON + ", \"green\": " + g;
  colorJSON = colorJSON + ", \"blue\": " + b + "}";
  if (mode == 3) {
    colorJSON = colorJSON + ",{\"ww\": " + (ww2);
    colorJSON = colorJSON + ", \"red\": " + r2;
    colorJSON = colorJSON + ", \"green\": " + g2;
    colorJSON = colorJSON + ", \"blue\": " + b2 + "}";
  }
  colorJSON = colorJSON + "]}";
  server.send(200, "text/json", colorJSON);
}

void getChipId() {
  // chipId = "{\"id\": \"" + ESP.getChipId() + "\"}";
  // server.send(200, "text/json", chipId);
}
 
// Define routing
void restServerRouting() {
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html",
            "Welcome to the REST Web Server");
    });
    server.on("/off", HTTP_PUT, putPowerOff);
    server.on("/on", HTTP_PUT, putPowerOn);
    server.on("/rainbow", HTTP_PUT, putRainbow);
    server.on("/color", HTTP_PUT, putColor);
    server.on("/gradient", HTTP_PUT, putGradient);

    server.on("/power", HTTP_GET, getPower);
    server.on("/rainbow", HTTP_GET, getRainbow);
    server.on("/color", HTTP_GET, getColor);

}
 
// Manage not found URL
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}