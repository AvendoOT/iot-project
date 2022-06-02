idf_component_register(SRCS "808H5V5.cpp" "adc.cpp" "main.cpp" "CLed.cpp"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "certs/AmazonRootCA1.pem"
                                   "certs/certificate.pem.crt"
                                   "certs/private.pem.key")
