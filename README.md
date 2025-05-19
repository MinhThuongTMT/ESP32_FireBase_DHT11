# ESP32_FireBase_DHT11

> **Giám sát nhiệt độ & độ ẩm real‑time với ESP32, DHT11, LCD I²C và Firebase Realtime Database**

![Banner](./assets/banner.png)

---

## ✨ Tính năng chính

- **Kết nối Wi‑Fi tự động** với **WiFiManager** – cấu hình qua Captive Portal, không cần nạp lại firmware.  
- **Đọc nhiệt độ & độ ẩm** từ **DHT11** mỗi 5 giây và hiển thị trên **LCD 16 × 2 I²C**.  
- **Đồng bộ dữ liệu real‑time** lên **Firebase Realtime Database** (mỗi 30 giây) → dễ dàng theo dõi & vẽ biểu đồ Web/App.  
- **Xử lý mất kết nối** bằng **Exponential Backoff**, tự động làm mới token & reconnect.

---

## 📁 Mục lục

1. [Sơ đồ hệ thống](#sơ-đồ-hệ-thống)  
2. [Phần cứng](#phần-cứng)  
3. [Phần mềm](#phần-mềm)  
4. [Cài đặt](#cài-đặt)  
5. [Cấu hình Firebase](#cấu-hình-firebase)  
6. [Triển khai](#triển-khai)  
7. [Cấu trúc dự án](#cấu-trúc-dự-án)  
8. [API Realtime Database](#api-realtime-database)  
9. [Khắc phục sự cố](#khắc-phục-sự-cố)  
10. [Roadmap](#roadmap)  
11. [License](#license)

---

## Sơ đồ hệ thống

```mermaid
graph LR
  DHT11 -->|Temp & Hum| ESP32
  ESP32 -->|LCD I²C| LCD["LCD 16x2"]
  ESP32 -->|Wi‑Fi| Router
  Router -->|HTTPS| Firebase[(Realtime DB)]
```
## 🛠️ Cấu hình Firebase

🔹 Bước 1: Tạo Project trên Firebase

- **Truy cập Firebase Console, nhấn Add project, điền tên và hoàn tất các bước tạo mới.

- **Vào Build → Authentication → Sign-in method → Bật Anonymous để cho phép thiết bị ESP32 đăng nhập không cần tài khoản.

- **Vào Build → Realtime Database → Create Database:

- **Chọn location: Singapore (asia-southeast1)

- **Chọn chế độ: Start in test mode (cho phép đọc/ghi toàn quyền, có thể thay đổi sau)

🔹 Bước 2: Lấy Thông Tin Cấu Hình Firebase

- **Nhấn vào biểu tượng ⚙ Project Settings (góc trái trên cùng)

- **Trong tab General, kéo xuống phần Your apps, chọn </> Web App → tạo mới

Sao chép các thông tin sau:
json
```mermaid
{
  "apiKey": "AIzaSyBiYP9n3SArwd1J-yrWoJlVHAEUeB5oCsQ",
  "authDomain": "aiotweb-ce263.firebaseapp.com",
  "databaseURL": "https://aiotweb-ce263-default-rtdb.asia-southeast1.firebasedatabase.app",
  "projectId": "aiotweb-ce263",
  "storageBucket": "aiotweb-ce263.firebasestorage.app",
  "messagingSenderId": "1050959934296",
  "appId": "1:1050959934296:web:d87440c7f0188f8f886b2d",
  "measurementId": "G-BQTT1E21JB"
}
```
Ghi lại databaseURL và apiKey để cấu hình trong chương trình ESP32.

🔹 Bước 3: (Tuỳ chọn) Kết nối Firebase với Web App
Bạn có thể sử dụng Firebase JS SDK để hiển thị dữ liệu từ ESP32 theo thời gian thực lên giao diện web:

html
Sao chép
Chỉnh sửa
<script type="module">
  import { initializeApp } from "https://www.gstatic.com/firebasejs/11.7.3/firebase-app.js";
  import { getAnalytics } from "https://www.gstatic.com/firebasejs/11.7.3/firebase-analytics.js";

  const firebaseConfig = {
    apiKey: "AIzaSyBiYP9n3SArwd1J-yrWoJlVHAEUeB5oCsQ",
    authDomain: "aiotweb-ce263.firebaseapp.com",
    databaseURL: "https://aiotweb-ce263-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "aiotweb-ce263",
    storageBucket: "aiotweb-ce263.firebasestorage.app",
    messagingSenderId: "1050959934296",
    appId: "1:1050959934296:web:d87440c7f0188f8f886b2d",
    measurementId: "G-BQTT1E21JB"
  };

  const app = initializeApp(firebaseConfig);
  const analytics = getAnalytics(app);
</script>
✅ Sau khi hoàn tất, bạn có thể upload chương trình lên ESP32. Thiết bị sẽ:

Kết nối Wi‑Fi qua WiFiManager

Đọc nhiệt độ & độ ẩm từ DHT11

Hiển thị trên LCD

Gửi dữ liệu lên Firebase Realtime Database theo chu kỳ 30 giây.

Nếu bạn muốn, mình có thể viết tiếp phần Triển khai, Cấu trúc dự án, hoặc API Realtime Database. Bạn chỉ cần nói nhé.







Bạn đã nói:
Định dạng phải có # 


ChatGPT đã nói:
Cảm ơn bạn! Dưới đây là phần # 5. Cấu hình Firebase được viết đúng định dạng Markdown như bạn yêu cầu, sử dụng tiêu đề # để đồng bộ với cấu trúc của README.md:

5. Cấu hình Firebase
🔹 Bước 1: Tạo Project trên Firebase
Truy cập Firebase Console, nhấn Add project, đặt tên project và hoàn tất quá trình tạo.

Vào Build → Authentication → Sign-in method, bật Anonymous để cho phép ESP32 đăng nhập không cần tài khoản.

Tiếp tục vào Build → Realtime Database → Create Database:

Location: Singapore (asia-southeast1)

Chế độ: Start in test mode (cho phép đọc/ghi toàn quyền – nên giới hạn sau khi phát triển xong)

🔹 Bước 2: Lấy thông tin cấu hình Firebase
Nhấn vào biểu tượng ⚙ Project settings (góc trái trên cùng)

Tại tab General, kéo xuống phần Your apps, nhấn </> để tạo Web App mới

Sau khi tạo, bạn sẽ nhận được cấu hình như sau:

json
Sao chép
Chỉnh sửa
{
  "apiKey": "AIzaSyBiYP9n3SArwd1J-yrWoJlVHAEUeB5oCsQ",
  "authDomain": "aiotweb-ce263.firebaseapp.com",
  "databaseURL": "https://aiotweb-ce263-default-rtdb.asia-southeast1.firebasedatabase.app",
  "projectId": "aiotweb-ce263",
  "storageBucket": "aiotweb-ce263.firebasestorage.app",
  "messagingSenderId": "1050959934296",
  "appId": "1:1050959934296:web:d87440c7f0188f8f886b2d",
  "measurementId": "G-BQTT1E21JB"
}
📌 Lưu lại databaseURL và apiKey để cấu hình trong code ESP32.

🔹 Bước 3: (Tuỳ chọn) Kết nối Firebase với Web App
Nếu bạn muốn hiển thị dữ liệu từ Firebase trên trang web real-time, thêm đoạn mã sau vào HTML:

html
Sao chép
Chỉnh sửa
<script type="module">
  import { initializeApp } from "https://www.gstatic.com/firebasejs/11.7.3/firebase-app.js";
  import { getAnalytics } from "https://www.gstatic.com/firebasejs/11.7.3/firebase-analytics.js";

  const firebaseConfig = {
    apiKey: "AIzaSyBiYP9n3SArwd1J-yrWoJlVHAEUeB5oCsQ",
    authDomain: "aiotweb-ce263.firebaseapp.com",
    databaseURL: "https://aiotweb-ce263-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "aiotweb-ce263",
    storageBucket: "aiotweb-ce263.firebasestorage.app",
    messagingSenderId: "1050959934296",
    appId: "1:1050959934296:web:d87440c7f0188f8f886b2d",
    measurementId: "G-BQTT1E21JB"
  };

  const app = initializeApp(firebaseConfig);
  const analytics = getAnalytics(app);
</script>
✅ Sau khi hoàn tất:

ESP32 sẽ tự động kết nối Wi‑Fi bằng WiFiManager

Đọc dữ liệu từ cảm biến DHT11 mỗi 5 giây

Hiển thị lên LCD

Gửi dữ liệu lên Firebase Realtime Database mỗi 30 giây
