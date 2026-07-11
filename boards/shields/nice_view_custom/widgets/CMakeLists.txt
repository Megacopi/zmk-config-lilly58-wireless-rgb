if(CONFIG_ZMK_DISPLAY)
    target_sources(app PRIVATE
        main_display.c
        battery_status.c
        util.c
        ../assets/battery.c
        ../assets/battery_mask.c
        ../assets/bolt.c
        ../assets/font.c
    )
endif()
