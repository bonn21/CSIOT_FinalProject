#include<Arduino.h>

// chuỗi này để lưu trữ html
const char index_html[] PROGMEM = R"HoaiBac(
<!DOCTYPE html>
<html>

<head>
    <meta charset='utf-8'>
    <meta http-equiv='X-UA-Compatible' content='IE=edge'>
    <title>SmartHome - Home</title>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <link rel='stylesheet' type='text/css' media='screen' href='main.css'>
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css"
        integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <script src='https://kit.fontawesome.com/a076d05399.js' crossorigin='anonymous'></script>
    <script src='main.js'></script>
    <style>
        body {
            background-color: #f2f5f5;
            font: 14px sans-serif;
            margin-top: 40px;
            margin-left: 10px;
            margin-right: 10px;
        }
        .header {
            text-align: center;
            color: #ef3232;
            margin-bottom: 15px;
            font-size:45px;
        }

        .control input[type="text"] {
            margin-left: 20px;
            background-color: transparent;
            border: none;
            color: #6a1d1d;
            font-size: 30px;
            width: 50px;
            text-align: center;
        }

        .control {
            display: flex;
            justify-content: center;
            margin-top: 10px;
        }

        .bt {
            width: 100px;
            height: 50px;
        }

        .mb {
            text-align: center;
        }

        .auto {
            display: flex;
            justify-content: center;
        }

        .button {
        border: none;
        border-radius: 25px;
        color: white;
        padding: 15px 32px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 20px;
        cursor: pointer;
        width: 160px;
        height: 80px;
        size: 32px;
        }

        .buttonauto {
        border: none;
        border-radius: 25px;
        color: white;
        background-color: #86D293;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 20px;
        cursor: pointer;
        width: 80px;
        height: 40px;
        size: 32px;
        }

        .btnOn {
        background-color: #4CAF50;
        }

        /* Green */
        .btnOff {
        background-color: #008CBA;
        }

        /* Blue */

        .btn_obj {
        display: flex;
        align-items: center;
        justify-content: center;

        /* border-radius: 25px; */
        }

        .text_style {
        font-size: 48px;
        font-weight: bold;
        /* text-decoration: solid; */
        }

        .units {
        font-size: 1.2rem;
        }
    </style>
    </head>

    <body>
    <h1 class="header">SMART HOME</h1>

    <div style="display: flex; align-items: center; justify-content: center;">
        <a href="/auto">
        <button class="buttonauto" style="margin-top: 10px;text-decoration: none;">Auto</button>
        </a>
    </div>
    
    <div style="display: flex; align-items: center; justify-content: center;  margin-top: 20px;">

        <div
        style="display: flex; height: 100px;  align-items: center; justify-content: center; width: 350px;text-align: center; margin-right: 200px;">
        <i class="fas fa-thermometer-half" style="color:red; font-size: 64px; margin-right: 10px;"></i>
        <p style="font-size: 32px;" class="text_style">Temp: </p>
        <span id="temperature" style="font-size: 32px;">%TEMPERATURE%</span>
        <sup style="font-size: 32px; font-weight: bold;">&deg;C</sup>
        </div>

        <div style="display: flex; height: 100px;  align-items: center; justify-content: center; width: 350px; text-align: center; margin-right: 200px;">
        <i class="fas fa-tint" style="color:#00add6; font-size: 64px; margin-right: 10px;"></i>
        <p style="font-size: 32px;" class="text_style">Humi: </p>
        <span id="humidity"  style="font-size: 32px;">%HUMIDITY%</span>
        <sup style="font-size: 32px; font-weight: bold;">&percnt;</sup>
        </div>
            
        <div style=" display: flex; height: 100px; align-items: center; justify-content: center; width: 350px; text-align: center;">

                <i class="fas fa-fire-alt" style="font-size:64px;color:orange;"></i>
                <p style="font-size: 32px;" class="text_style">Fire: </p>
                <span id="gas" style="font-size: 32px;">%GAS%</span>
                <sup style="font-size: 32px; font-weight: bold;">&percnt;</sup>
        </div>
    </div>
    <div style="display: flex; margin-top: 100px;" class="btn_obj">
        <div
        style="border-style: groove; border-radius: 25px; width: 350px; height: 400px; border-color: #D1E9F6; margin-right: 20px;">
        <p style="text-align: center;" class="text_style">Ban Công</p>
        <div class="btn_obj">
            <a href="/manual/balcony/open_door">
            <button class="button btnOn" style="margin-right: 20px;">Mở Cửa</button>
            </a>
        </div>
        <div class="btn_obj" style="margin-top: 20px;">
            <a href="/manual/balcony/cloes_door">
            <button class="button btnOff" style="margin-right: 20px;">Đóng Cửa</button>
            </a>
        </div>
        </div>

        <div style="border-style: groove; border-radius: 25px; width: 450px; height: 400px; border-color: #D1E9F6; margin-right: 20px;">
            <p style="text-align: center;" class="text_style">Phòng Khách</p>
            <div class="btn_obj">
                <a href="/manual/livingroom/ledon">
                <button class="button btnOn" style="margin-right: 10px; ">Bật Đèn</button>
                </a>
                
                <a href="/manual/livingroom/fanon">
                <button class="button btnOn" style="font-size: 20px; margin-right: 10px; border-radius: 25px; text-align: center;  width: 150px; height: 80px;">Bật Quạt</button>
                </a>
            </div>

            <div class="btn_obj" style="margin-top: 20px;">
                <a href="/manual/livingroom/ledoff">
                <button class="button btnOff" style="margin-right: 10px;">Tắt Đèn</button>
                </a>
                
                <a href="/manual/livingroom/fanoff">
                <button class="button btnOff" style="margin-right: 10px; border-radius: 25px; width: 150px; height: 80px;">Tắt Quạt</button>
                </a>

            </div>
        </div>

        <div style="border-style: groove; width: 350px; border-radius: 25px; height: 400px; border-style: groove; border-color: #D1E9F6; margin-right: 20px;">
            <div>
                <p style="text-align: center;" class="text_style">Phòng Ngủ</p>
                <div class="btn_obj">
                <a href="/manual/bedroom/ledon">
                    <button class="button btnOn">Bật Đèn</button>
                </a>

                </div>
                <div class="btn_obj" style="margin-top: 20px;">
                <a href="/manual/bedroom/ledoff">
                    <button class="button btnOff">Tắt Đèn</button>
                </a>
                </div>
            </div>
        </div>
        
        <div style="border-style: groove; border-radius: 25px; width: 450px; height: 400px; border-color: #D1E9F6; margin-right: 20px;">
            <p style="text-align: center;" class="text_style">Phòng Bếp</p>
                <div class="btn_obj">
                    <a href="/manual/garden/pumpon">
                    <button class="button btnOn" style="width: 150px; height: 80px; font-size: 20px; border-radius: 25px;">Bật Còi</button>
                    </a>

                    <a href="/manual/kitchen/fanon">
                    <button class="button btnOn" style="font-size: 20px; margin-left: 10px; border-radius: 25px; text-align: center; width: 150px; height: 80px;">Bật Quạt</button>
                    </a>
                </div>

                <div class="btn_obj" style="margin: 20px;">
                    <a href="/manual/garden/pumpoff">
                    <button class="button btnOff" style="width: 150px; height: 80px; font-size: 20px; border-radius: 25px;">Tắt Còi</button>
                    </a>

                    <a href="/manual/kitchen/fanoff">
                    <button class="button btnOff"
                        style="margin-left: 10px; border-radius: 25px; text-align:center;  width: 150px; height: 80px;">Tắt Quạt</button>
                    </a>
                </div>
            </div>
        </div>
    </div>

</body>

</html>)HoaiBac";
