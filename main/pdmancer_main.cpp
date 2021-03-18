#include "pdmancer_main.hpp"
#include "./tcpm/tcpm.hpp"
#include <drv_fusb302.hpp>
#include <esp_log.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TAG "fusb302-main"

extern "C" void app_main(void)
{
    pd_main main;
    main.start();
    vTaskDelay(portMAX_DELAY);
}

void pd_main::start()
{
    auto fusb302 = device::fusb302(32, 33, 35);
    auto tcpm = protocol::tcpm(fusb302);

    fusb302.on_pkt_received([&]() -> int {
        fusb302.receive_pkt(&header, data_objs, sizeof(data_objs));
        ESP_LOGI(TAG, "Header: 0x%x, data: 0x%x", header, data_objs[0]);
        return ESP_OK;
    });
    while (true) {
        fusb302.auto_config_polarity();
        tcpm.perform_sink();
        // tcpm.send_request_fixed(7000, 1000);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    vTaskDelay(portMAX_DELAY);
}

int pd_main::on_rx()
{
    return 0;
}
