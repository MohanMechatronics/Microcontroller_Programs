#include "esp_http_server.h"
#include "esp_camera.h"
#include "esp_wifi.h"
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"


/* Wifi Crdentials */
const char* ssid = "Mr.BGM";
const char* password = "2444666668888888";


#define CAMERA_MODEL_AI_THINKER

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void startCameraServer();

// MOTORS GPIO
#define B_LEFT 12
#define B_RIGHT 13
#define F_LEFT 15
#define F_RIGHT 14
#define ALERT 2
const int FlashPin = 4;


#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

static esp_err_t stream_handler(httpd_req_t *req) {
  Serial.print("stream_handler running on core ");
  Serial.println(xPortGetCoreID());
  
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t * _jpg_buf = NULL;
  char * part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if (res != ESP_OK) {
    return res;
  }

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      if(fb->width > 200){
        if (fb->format != PIXFORMAT_JPEG) {
          bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
          esp_camera_fb_return(fb);
          fb = NULL;
          if (!jpeg_converted) {
            Serial.println("JPEG compression failed");
            res = ESP_FAIL;
          }
        } else {
          _jpg_buf_len = fb->len;
          _jpg_buf = fb->buf;
        }
      }
    }
    if (res == ESP_OK) {
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    if (fb) {
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if (_jpg_buf) {
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    if (res != ESP_OK) {
      break;
    }
  }
  return res;
}


static esp_err_t cmd_handler(httpd_req_t *req)
{
  char*  buf;
  size_t buf_len;
  char variable[32] = {0,};
  char value[32] = {0,};

  buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
    buf = (char*)malloc(buf_len);
    if (!buf) {
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      if (httpd_query_key_value(buf, "var", variable, sizeof(variable)) == ESP_OK &&
          httpd_query_key_value(buf, "val", value, sizeof(value)) == ESP_OK) {
      } else {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
      }
    } else {
      free(buf);
      httpd_resp_send_404(req);
      return ESP_FAIL;
    }
    free(buf);
  } else {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }

  int val = atoi(value);
  sensor_t * s = esp_camera_sensor_get();
  int res = 0;

  if (!strcmp(variable, "framesize"))
  {
    Serial.println("framesize");
    if (s->pixformat == PIXFORMAT_JPEG) res = s->set_framesize(s, (framesize_t)val);
  }
  else if (!strcmp(variable, "quality"))
  {
    Serial.println("quality");
    res = s->set_quality(s, val);
  }
  //Remote Control Car
  //Don't use channel 1 and channel 2
  else if (!strcmp(variable, "flash"))
  {
    ledcWrite(7, val);
  }
  else if (!strcmp(variable, "car")) {
    if (val == 1) {
      Serial.println("Forward");
      digitalWrite(B_LEFT, HIGH);
      digitalWrite(B_RIGHT, HIGH);
    }
    else if (val == 2) {
      Serial.println("Turn Left");
      digitalWrite(B_RIGHT, HIGH);
      digitalWrite(F_LEFT, HIGH);      
      
    }
    else if (val == 3) {
      Serial.println("Stop");
      digitalWrite(B_LEFT, LOW);
      digitalWrite(B_RIGHT, LOW);
      digitalWrite(F_LEFT, LOW);
      digitalWrite(F_RIGHT, LOW);
    }
    else if (val == 4) {
      Serial.println("Turn Right");
      digitalWrite(B_LEFT, HIGH);
      digitalWrite(F_RIGHT, HIGH);
    }
    else if (val == 5) {
      Serial.println("Backward");
      digitalWrite(F_RIGHT, HIGH);
      digitalWrite(F_LEFT, HIGH);
    }
    else if (val == 6) {
      Serial.println("ALERT ON");
      digitalWrite(ALERT, HIGH);
    }
    else if (val == 7) {
      Serial.println("ALERT OFF");
      digitalWrite(ALERT,LOW);
    }
  }
  else
  {
    Serial.println("variable");
    res = -1;
  }

  if (res) {
    return httpd_resp_send_500(req);
  }

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, NULL, 0);
}



/* Index page */
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!doctype html>
<html>
    <head>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width,initial-scale=1">
        <title>HAVE FUN WITH YOUR CONTROLS </title>
        <style>
            .button {background-color: #0097b5;border: none;border-radius: 4px;color: white;padding: 10px 25px;text-align: center;font-size: 16px;margin: 4px 2px;cursor: pointer;}
            .slider {appearance: none;width: 70%;height: 15px;border-radius: 10px;background: #d3d3d3;outline: none;}
            .slider::-webkit-slider-thumb {appearance: none;appearance: none;width: 30px;height: 30px;border-radius: 50%;background: #0097b5;}
            .label {color: #0097b5;font-size: 18px;}
            /* ----------------------------------- Stream Viewer */
            img {
                width: auto ;
                max-width: 100% ;
                height: auto ; 
            }
            /* ----------------------------------- */
.switch {
  position: relative;
  display: inline-block;
  width: 60px;
  height: 34px;
}

.switch input { 
  outline: 0;
  opacity: 0;
  width: 0;
  height: 0;
}

.slider2 {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: #ccc;
  -webkit-transition: .4s;
  transition: .4s;
}

.slider2:before {
  position: absolute;
  content: "";
  height: 26px;
  width: 26px;
  left: 4px;
  bottom: 4px;
  background-color: white;
  -webkit-transition: .4s;
  transition: .4s;
}

input:checked + .slider2 {
  background-color: #2196F3;
}

input:focus + .slider2 {
  box-shadow: 0 0 1px #2196F3;
}

input:checked + .slider2:before {
  -webkit-transform: translateX(26px);
  -ms-transform: translateX(26px);
  transform: translateX(26px);
}

/* Rounded sliders */
.slider2.round {
  border-radius: 34px;
}

.slider2.round:before {
  border-radius: 50%;
}
        /* ----------------------------------- */
        </style>
    </head>
    <body>
    <div align=center> <img src="" id="vdstream"></div>
    <br/>
    <br/>
    <div align=center> 
    <button class="button" id="forward" ontouchstart="fetch(document.location.origin+'/control?var=car&val=1');" ontouchend="fetch(document.location.origin+'/control?var=car&val=3');" >Forward</button>
    </div>
    <br/>
    <div align=center> 
    <button class="button" id="turnleft" ontouchstart="fetch(document.location.origin+'/control?var=car&val=2');" ontouchend="fetch(document.location.origin+'/control?var=car&val=3');" >Turn Left</button>
    <button class="button" id="stop" onclick="fetch(document.location.origin+'/control?var=car&val=3');">Stop</button>
    <button class="button" id="turnright" ontouchstart="fetch(document.location.origin+'/control?var=car&val=4');" ontouchend="fetch(document.location.origin+'/control?var=car&val=3');" >Turn Right</button>
    </div>
    <br/>
    <div align=center> 
    <button class="button" id="backward" ontouchstart="fetch(document.location.origin+'/control?var=car&val=5');" ontouchend="fetch(document.location.origin+'/control?var=car&val=3');">Backward</button>
    </div>
    <br/>
    <div align=center> 
    <label class="label">Flash</label>
    <input type="range" class="slider" id="flash" min="0" max="255" value="0" onchange="try{fetch(document.location.origin+'/control?var=flash&val='+this.value);}catch(e){}">
    </div>
    <br/>
    <label for="controlstate" class="switch">
      <input type="checkbox" id="controlstate" onclick="myFunction()">
      <span class="slider2 round" ></span>
    </label>

    <script>
    /* ----------------------------------- Calls the video stream link and displays it */ 
    window.onload = document.getElementById("vdstream").src = window.location.href.slice(0, -1) + ":81/stream";
    /* ----------------------------------- */
    function myFunction() {
     var checkBox = document.getElementById("controlstate");
     if (checkBox.checked == true){
      fetch(document.location.origin+'/control?var=car&val=6');
     } else {
      fetch(document.location.origin+'/control?var=car&val=7');
     }
   }
  /* ----------------------------------- */
    </script>   
    </body>
</html>
)rawliteral";

static esp_err_t index_handler(httpd_req_t *req){
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
}

void startCameraServer(){
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;                                  
    
    httpd_uri_t index_uri = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = index_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t cmd_uri = {
        .uri       = "/control",
        .method    = HTTP_GET,
        .handler   = cmd_handler,
        .user_ctx  = NULL
    };

   httpd_uri_t stream_uri = {
        .uri       = "/stream",
        .method    = HTTP_GET,
        .handler   = stream_handler,
        .user_ctx  = NULL
    };
    
    if (httpd_start(&camera_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(camera_httpd, &index_uri);
        httpd_register_uri_handler(camera_httpd, &cmd_uri);
    }

    config.server_port += 1;
    config.ctrl_port += 1;
    if (httpd_start(&stream_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(stream_httpd, &stream_uri);
    }
    Serial.println();
    Serial.println("Camera Server started successfully");
    Serial.print("Camera Stream Ready! Go to: http://");
    Serial.println(WiFi.localIP());
    Serial.println();
}

IPAddress local_IP(192, 168, 1, 199);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

void setup() {
  
  // Disable brownout detector.
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  
  pinMode(B_LEFT, OUTPUT);
  pinMode(B_RIGHT, OUTPUT);
  pinMode(F_LEFT, OUTPUT);
  pinMode(F_RIGHT, OUTPUT);
  pinMode(ALERT, OUTPUT);    
  
  Serial.println("Start configuring and initializing the camera...");
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 15;
  config.fb_count = 1;

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  //drop down frame size for higher initial frame rate
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);

  
  WiFi.softAP(ssid,password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  startCameraServer();/* Starting camera server */
  
}

void loop() {
  // put your main code here, to run repeatedly:

}

