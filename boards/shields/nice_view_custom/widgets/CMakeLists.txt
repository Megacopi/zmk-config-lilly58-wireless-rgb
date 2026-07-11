if(CONFIG_ZMK_DISPLAY)
    target_sources(app PRIVATE
        main_display.c
        battery_status.c
        util.c
        ../assets/bolt.c
    )
endif()
