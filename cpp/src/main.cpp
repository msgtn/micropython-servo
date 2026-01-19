#include <cstdio>
#include <cstring>
#include <map>
#include <string>

#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "pico/stdlib.h"

#include "robot.hpp"
#include "sensors/hcsr04.hpp"

// Pin configurations
constexpr uint UART_CMD_TX_PIN = 4;
constexpr uint UART_CMD_RX_PIN = 5;
constexpr uint UART_CMD_BAUD = 115200;

constexpr uint UART_DXL_TX_PIN = 0;
constexpr uint UART_DXL_RX_PIN = 1;
constexpr uint UART_DXL_BAUD = 57600;

constexpr uint HCSR04_TRIGGER_PIN = 15;
constexpr uint HCSR04_ECHO_PIN = 14;

// Command buffer
constexpr size_t CMD_BUFFER_SIZE = 1024;
char cmd_buffer[CMD_BUFFER_SIZE];
size_t cmd_buffer_idx = 0;

// Parse command string like "1=512&2=1024&3=2048" into a map
std::map<int, float> parseCmdDict(const char *msg) {
  std::map<int, float> result;

  if (msg == nullptr || *msg == '\0') {
    return result;
  }

  // Make a copy to tokenize
  char buffer[CMD_BUFFER_SIZE];
  strncpy(buffer, msg, CMD_BUFFER_SIZE - 1);
  buffer[CMD_BUFFER_SIZE - 1] = '\0';

  // Split by '&'
  char *saveptr1;
  char *token = strtok_r(buffer, "&", &saveptr1);

  while (token != nullptr) {
    // Split by '='
    char *equals = strchr(token, '=');
    if (equals != nullptr) {
      *equals = '\0';
      const char *id_str = token;
      const char *val_str = equals + 1;

      if (*id_str != '\0' && *val_str != '\0') {
        int motor_id = atoi(id_str);
        float value = static_cast<float>(atof(val_str));
        result[motor_id] = value;
      }
    }

    token = strtok_r(nullptr, "&", &saveptr1);
  }

  // Print parsed result
//   printf("Parsed %zu entries: ", result.size());
//   for (const auto& [id, val] : result) {
//       printf("%d=%.2f ", id, val);
//   }
//   printf("\n");

  return result;
}

// Trim whitespace from string
void trimString(char *str) {
  if (str == nullptr)
    return;

  // Trim leading whitespace
  char *start = str;
  while (*start && (*start == ' ' || *start == '\t' || *start == '\r' ||
                    *start == '\n')) {
    start++;
  }

  // Trim trailing whitespace
  char *end = start + strlen(start) - 1;
  while (end > start &&
         (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) {
    *end = '\0';
    end--;
  }

  // Move trimmed string to beginning if needed
  if (start != str) {
    memmove(str, start, strlen(start) + 1);
  }
}

int main() {
  // Initialize stdio (USB)
  stdio_init_all();

  // Wait a bit for USB to connect
  sleep_ms(1000);

  // Initialize LED
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
  gpio_put(PICO_DEFAULT_LED_PIN, 0);

  // Initialize command UART (UART1)
  uart_init(uart1, UART_CMD_BAUD);
  gpio_set_function(UART_CMD_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_CMD_RX_PIN, GPIO_FUNC_UART);
  uart_set_fifo_enabled(uart1, true);

#ifdef USE_DYNAMIXEL
  // Initialize Dynamixel robot (uses UART0)
  DynamixelRobot robot(uart0, UART_DXL_TX_PIN, UART_DXL_RX_PIN, UART_DXL_BAUD);
  if (!robot.init()) {
    printf("Failed to initialize Dynamixel robot\n");
  }
  robot.addMotor(1);
  robot.addMotor(2);
  robot.addMotor(3);
  robot.addMotor(4);
#else
  // Initialize PWM servo robot
  Robot robot;
  robot.addServo(1, 6);
  robot.addServo(2, 7);
  robot.addServo(3, 10);
  robot.addServo(4, 11);
#endif

  // Initialize HC-SR04 distance sensor (optional - uncomment to enable)
  sensors::HCSR04 distance_sensor(HCSR04_TRIGGER_PIN, HCSR04_ECHO_PIN);
  uint32_t last_distance_time = 0;
  constexpr uint32_t DISTANCE_INTERVAL_MS = 100;

  printf("Servo controller ready\n");

  // Main loop
  while (true) {
    // Check for commands from UART1
    while (uart_is_readable(uart1)) {
      char c = uart_getc(uart1);

      if (c == '\n') {
        // Process complete command
        cmd_buffer[cmd_buffer_idx] = '\0';
        trimString(cmd_buffer);

        if (cmd_buffer_idx > 0) {
          gpio_put(PICO_DEFAULT_LED_PIN, 1);

          auto cmd_dict = parseCmdDict(cmd_buffer);
          robot.writeMotorStates(cmd_dict);

          gpio_put(PICO_DEFAULT_LED_PIN, 0);
        }

        cmd_buffer_idx = 0;
      } else if (cmd_buffer_idx < CMD_BUFFER_SIZE - 1) {
        cmd_buffer[cmd_buffer_idx++] = c;
      }
    }

    // Check for commands from USB stdio (stdin)
    int c = getchar_timeout_us(0);
    if (c != PICO_ERROR_TIMEOUT) {
      if (c == '\n' || c == '\r') {
        // Process complete command
        cmd_buffer[cmd_buffer_idx] = '\0';
        trimString(cmd_buffer);

        if (cmd_buffer_idx > 0) {
          gpio_put(PICO_DEFAULT_LED_PIN, 1);

          auto cmd_dict = parseCmdDict(cmd_buffer);
          robot.writeMotorStates(cmd_dict);

          gpio_put(PICO_DEFAULT_LED_PIN, 0);
        }

        cmd_buffer_idx = 0;
      } else if (cmd_buffer_idx < CMD_BUFFER_SIZE - 1) {
        cmd_buffer[cmd_buffer_idx++] = static_cast<char>(c);
      }
    }

    // Optional: Periodic distance sensor reading
    // Uncomment the following block to enable distance sensor output
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now - last_distance_time >= DISTANCE_INTERVAL_MS) {
      int32_t distance_mm = distance_sensor.distanceMm();
      char uart_buffer[32];
      if (distance_mm >= 0) {
        snprintf(uart_buffer, sizeof(uart_buffer), "%d\n", distance_mm);
        printf("%s", uart_buffer);
        uart_puts(uart1, uart_buffer);
      } else {
        snprintf(uart_buffer, sizeof(uart_buffer), "sensor error\n");
        printf("%s", uart_buffer);
        uart_puts(uart1, uart_buffer);
      }
      last_distance_time = now;
    }

    // Small delay to avoid busy-waiting
    sleep_us(10);
  }

  return 0;
}
