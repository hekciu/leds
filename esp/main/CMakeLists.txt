idf_component_register(SRCS "main.c"
                    PRIV_REQUIRES spi_flash
                    PRIV_REQUIRES driver 
                    PRIV_REQUIRES bt 
                    PRIV_REQUIRES nvs_flash 
                    INCLUDE_DIRS "")
