// شامل کردن کتابخانه Wire برای ارتباط I2C با PCF8574
#include <Wire.h>

// آدرس I2C ماژول PCF8574
// این آدرس ممکن است بسته به ماژول شما 0x27 یا 0x3F باشد.
// اگر مطمئن نیستید، می‌توانید یک اسکنر I2C را اجرا کنید تا آدرس صحیح را پیدا کنید.
#define PCF8574_ADDRESS 0x27

// --- تعریف پین‌های ورودی برای پوش‌باتن‌های مستقیم ---
// این پین‌ها به عنوان INPUT_PULLUP تنظیم می‌شوند، به این معنی که در حالت عادی HIGH هستند و با فشار دادن دکمه LOW می‌شوند.
#define DIRECT_BUTTON_1_PIN 2 // باتون 1 متصل به پین D2
#define DIRECT_BUTTON_2_PIN 3 // باتون 2 متصل به پین D3
#define DIRECT_BUTTON_3_PIN 4 // باتون 3 متصل به پین D4
#define DIRECT_BUTTON_4_PIN 5 // باتون 4 متصل به پین D5

// --- تعریف پین‌های خروجی برای رله‌های مستقیم ---
#define DIRECT_RELAY_1_PIN 6 // خروجی 1 متصل به پین D6
#define DIRECT_RELAY_2_PIN 7 // خروجی 2 متصل به پین D7
#define DIRECT_RELAY_3_PIN 8 // خروجی 3 متصل به پین D8
#define DIRECT_RELAY_4_PIN 9 // خروجی 4 متصل به پین D9

// --- تعریف پین‌های خروجی برای رله‌های کنترل شده با PCF8574 ---
#define PCF_RELAY_D10 10
#define PCF_RELAY_D11 11
#define PCF_RELAY_D12 12
#define PCF_RELAY_D13 13
#define PCF_RELAY_A3 A3 // پین آنالوگ A3 می‌تواند به عنوان پین دیجیتال نیز استفاده شود

void setup() {
  // راه‌اندازی ارتباط سریال برای اشکال‌زدایی (اختیاری)
  Serial.begin(9600);
  Serial.println("برنامه کنترل ترکیبی رله آردوینو آغاز شد!");

  // راه‌اندازی ارتباط I2C برای PCF8574
  Wire.begin();

  // تنظیم پین‌های دکمه‌های مستقیم به عنوان ورودی با مقاومت پول‌آپ داخلی
  pinMode(DIRECT_BUTTON_1_PIN, INPUT_PULLUP);
  pinMode(DIRECT_BUTTON_2_PIN, INPUT_PULLUP);
  pinMode(DIRECT_BUTTON_3_PIN, INPUT_PULLUP);
  pinMode(DIRECT_BUTTON_4_PIN, INPUT_PULLUP);

  // تنظیم پین‌های رله‌های مستقیم به عنوان خروجی
  pinMode(DIRECT_RELAY_1_PIN, OUTPUT);
  pinMode(DIRECT_RELAY_2_PIN, OUTPUT);
  pinMode(DIRECT_RELAY_3_PIN, OUTPUT);
  pinMode(DIRECT_RELAY_4_PIN, OUTPUT);

  // تنظیم پین‌های رله‌های کنترل شده با PCF8574 به عنوان خروجی
  pinMode(PCF_RELAY_D10, OUTPUT);
  pinMode(PCF_RELAY_D11, OUTPUT);
  pinMode(PCF_RELAY_D12, OUTPUT);
  pinMode(PCF_RELAY_D13, OUTPUT);
  pinMode(PCF_RELAY_A3, OUTPUT);

  // اطمینان از اینکه همه رله‌ها در ابتدا خاموش هستند
  digitalWrite(DIRECT_RELAY_1_PIN, LOW);
  digitalWrite(DIRECT_RELAY_2_PIN, LOW);
  digitalWrite(DIRECT_RELAY_3_PIN, LOW);
  digitalWrite(DIRECT_RELAY_4_PIN, LOW);
  digitalWrite(PCF_RELAY_D10, LOW);
  digitalWrite(PCF_RELAY_D11, LOW);
  digitalWrite(PCF_RELAY_D12, LOW);
  digitalWrite(PCF_RELAY_D13, LOW); // خطای 'Write' به 'digitalWrite' اصلاح شد
  digitalWrite(PCF_RELAY_A3, LOW);
}

void loop() {
  // --- بخش کنترل رله‌ها با PCF8574 (منطق پیچیده) ---
  // وضعیت مطلوب برای هر رله، که در ابتدای هر حلقه به LOW (خاموش) تنظیم می‌شود.
  bool pcf_relay10_desired_state = LOW;
  bool pcf_relay11_desired_state = LOW;
  bool pcf_relay12_desired_state = LOW;
  bool pcf_relay13_desired_state = LOW;
  bool pcf_relayA3_desired_state = LOW;

  byte pcf8574_input_state = 0; // متغیری برای ذخیره وضعیت 8 پین PCF8574

  // خواندن وضعیت 8 پین PCF8574 از طریق I2C
  Wire.requestFrom(PCF8574_ADDRESS, 1); // درخواست 1 بایت (8 بیت) از PCF8574
  if (Wire.available()) { // اگر داده‌ای در دسترس است
    pcf8574_input_state = Wire.read(); // خواندن بایت وضعیت
  } else {
    Serial.println("خطا در خواندن از PCF8574!");
    // در صورت بروز خطا، رله‌ها خاموش می‌مانند.
  }

  // خواندن وضعیت دکمه‌ها از طریق PCF8574
  // فرض بر این است که دکمه‌ها به گونه‌ای سیم‌کشی شده‌اند که با فشرده شدن، پین مربوطه در PCF8574 LOW می‌شود.
  // ! برای معکوس کردن منطق استفاده می‌شود، به طوری که LOW (فشرده) به true تبدیل شود.
  bool btn_pcf_P0_pressed = !((pcf8574_input_state >> 0) & 0x01); // پین P0 ماژول PCF8574
  bool btn_pcf_P1_pressed = !((pcf8574_input_state >> 1) & 0x01); // پین P1 ماژول PCF8574
  bool btn_pcf_P2_pressed = !((pcf8574_input_state >> 2) & 0x01); // پین P2 ماژول PCF8574
  bool btn_pcf_P3_pressed = !((pcf8574_input_state >> 3) & 0x01); // پین P3 ماژول PCF8574
  bool btn_pcf_P4_pressed = !((pcf8574_input_state >> 4) & 0x01); // پین P4 ماژول PCF8574
  bool btn_pcf_P5_pressed = !((pcf8574_input_state >> 5) & 0x01); // پین P5 ماژول PCF8574
  bool btn_pcf_P6_pressed = !((pcf8574_input_state >> 6) & 0x01); // پین P6 ماژول PCF8574
  bool btn_pcf_P7_pressed = !((pcf8574_input_state >> 7) & 0x01); // پین P7 ماژول PCF8574

  // منطق برای کنترل رله‌های D10 و D11 (با P0 و P1)
  if (btn_pcf_P0_pressed || btn_pcf_P1_pressed) {
    pcf_relay11_desired_state = HIGH;
  }
  // منطق برای کنترل رله‌های D10 و D12 (با P2 و P3)
  if (btn_pcf_P2_pressed || btn_pcf_P3_pressed) {
    pcf_relay12_desired_state = HIGH;
  }
  // منطق برای کنترل رله‌های D10 و D13 (با P4 و P5)
  if (btn_pcf_P4_pressed || btn_pcf_P5_pressed) {
    pcf_relay13_desired_state = HIGH;
  }
  // منطق برای کنترل رله‌های D10 و A3 (با P6 و P7)
  if (btn_pcf_P6_pressed || btn_pcf_P7_pressed) {
    pcf_relayA3_desired_state = HIGH;
  }
  // منطق برای رله D10 (روشن شدن با هر "کلید شماره یک")
  if (btn_pcf_P0_pressed || btn_pcf_P2_pressed || btn_pcf_P4_pressed || btn_pcf_P6_pressed) {
    pcf_relay10_desired_state = HIGH;
  }

  // اعمال وضعیت‌های تعیین شده به پین‌های رله‌های PCF8574
  digitalWrite(PCF_RELAY_D10, pcf_relay10_desired_state);
  digitalWrite(PCF_RELAY_D11, pcf_relay11_desired_state);
  digitalWrite(PCF_RELAY_D12, pcf_relay12_desired_state);
  digitalWrite(PCF_RELAY_D13, pcf_relay13_desired_state);
  digitalWrite(PCF_RELAY_A3, pcf_relayA3_desired_state);

  // --- بخش کنترل رله‌ها با دکمه‌های مستقیم (منطق ساده) ---
  // خواندن وضعیت هر دکمه مستقیم
  // در حالت INPUT_PULLUP، فشار داده شدن دکمه به معنی LOW است.
  bool direct_button1_state = (digitalRead(DIRECT_BUTTON_1_PIN) == LOW);
  bool direct_button2_state = (digitalRead(DIRECT_BUTTON_2_PIN) == LOW);
  bool direct_button3_state = (digitalRead(DIRECT_BUTTON_3_PIN) == LOW);
  bool direct_button4_state = (digitalRead(DIRECT_BUTTON_4_PIN) == LOW);

  // کنترل رله‌های مستقیم بر اساس وضعیت دکمه‌ها
  // اگر دکمه فشرده شده باشد (true)، رله روشن می‌شود (HIGH).
  // اگر دکمه رها شده باشد (false)، رله خاموش می‌شود (LOW).
  digitalWrite(DIRECT_RELAY_1_PIN, direct_button1_state);
  digitalWrite(DIRECT_RELAY_2_PIN, direct_button2_state);
  digitalWrite(DIRECT_RELAY_3_PIN, direct_button3_state);
  digitalWrite(DIRECT_RELAY_4_PIN, direct_button4_state);

  // یک تاخیر کوتاه برای جلوگیری از لرزش دکمه‌ها (Debounce) و پایداری حلقه
  delay(50);
}
