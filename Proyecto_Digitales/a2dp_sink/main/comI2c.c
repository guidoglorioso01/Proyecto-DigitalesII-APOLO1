
#include "comI2c.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"
#include "volume_control.h"

#define DEBUG 1

static const char *TAG = "EepromSim";
static char *NAME_SPACE = "NVS_SPACE";

/////////////////////////////////////// Global variables ////////////////////////////////////////////////

uint8_t buff_rx[DATA_LENGTH] ;
uint8_t buff_tx[DATA_LENGTH] ;
UnionUserData local_user_data;
int8_t command_state;

nvs_handle_t app_nvs_handle;
const char* key = "DatosUser";
extern uint8_t correccion_loudness;
extern float loudness_target;
uint32_t tiempo_total=0;
uint32_t tiempo_actual=0;

extern uint8_t bt_state_global;

/////////////////////////////////////// Tasks ////////////////////////////////////////////////
void i2c_task (void* parameters){
    int8_t size = 0;

    while(1){
        size = i2c_slave_read_buffer(I2C_SLAVE_NUM, buff_rx, 1, portMAX_DELAY);

        if(size == ESP_FAIL){
            #if DEBUG
             printf("Fallo el Read\n");
             #endif
        }
        else{
            if(size == 0){
                #if DEBUG
                //printf("No se recibio mensaje\n");
                #endif
            }
            else{
                #if DEBUG
                printf("Se recibio el cmd: %i \n",buff_rx[0]);
                #endif
                command_state = anlyses_message(buff_rx[0]);
                //i2c_reset_tx_fifo(I2C_NUM_0);
                //i2c_reset_rx_fifo(I2C_NUM_0);

            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));

    }
        
}


/////////////////////////////////////// Functions ////////////////////////////////////////////////


static esp_err_t i2c_slave_init(void)
{
    i2c_set_pin(I2C_SLAVE_NUM, I2C_SLAVE_SDA_IO, I2C_SLAVE_SCL_IO, GPIO_PULLUP_DISABLE, GPIO_PULLUP_DISABLE,    I2C_MODE_SLAVE);

    int i2c_slave_port = I2C_SLAVE_NUM;
    i2c_config_t conf_slave = {
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .mode = I2C_MODE_SLAVE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = ESP_SLAVE_ADDR,
        .slave.maximum_speed = 100000,
    };
    esp_err_t err = i2c_param_config(i2c_slave_port, &conf_slave);
    if (err != ESP_OK) {
        return err;
    }
    //i2c_set_data_mode(I2C_SLAVE_NUM, I2C_DATA_MODE_LSB_FIRST, I2C_DATA_MODE_LSB_FIRST);
    return i2c_driver_install(i2c_slave_port, conf_slave.mode, I2C_SLAVE_RX_BUF_LEN, I2C_SLAVE_TX_BUF_LEN, 0);
}

uint8_t anlyses_message(uint8_t command){
    int8_t err = 0;
    static uint8_t progreso = 0;

    switch(command){
        case LOAD_CONFIG_CMD: // Se pasa la configuracion a guardar.
            vTaskDelay(pdMS_TO_TICKS(300)); // Espero 300 ms para recibir todo.

            err = i2c_slave_read_buffer(I2C_SLAVE_NUM, buff_rx,STRUCT_LENGHT, pdMS_TO_TICKS(100));

            #if DEBUG    
            printf("Se recibieron %i datos\n",err);
            #endif    

            if(err == 0)
                return ERROR;
            if(err !=STRUCT_LENGHT)
                return INSUFICIENT_DATA;

            memcpy(local_user_data.datosRaw, buff_rx, STRUCT_LENGHT);
            
            #if DEBUG
            print_data(local_user_data.user_data);
            #endif
        break;
        case GET_CONFIG_CMD: // Se pide la configuracion local
            i2c_reset_tx_fifo(I2C_SLAVE_NUM);
            err = i2c_slave_write_buffer(I2C_SLAVE_NUM, local_user_data.datosRaw,STRUCT_LENGHT,pdMS_TO_TICKS(100));

            if(err == ESP_FAIL)
                return ERROR;
        
        break;
        case STATE_COMAND_CMD: // Se pide el estado del comando anterior (si fue exitoso o no)
        
            if(command_state == ESP_OK)
                buff_tx[0] = COMAND_OK;
            else
                buff_tx[0] = COMAND_FAILED;

            i2c_reset_tx_fifo(I2C_SLAVE_NUM);
            err = i2c_slave_write_buffer(I2C_SLAVE_NUM, buff_tx,1,pdMS_TO_TICKS(100));
            
            vTaskDelay(pdMS_TO_TICKS(50)); // Espero 50 ms para reciba todo.

            #if DEBUG    
            printf("Se envia un %i \n",buff_tx[0]);
            #endif    
            
            if(err == ESP_FAIL)
                return ERROR;
            
        break;
        case SAVE_FLASH_CMD: // Guardar en flash datos
            err = write_nvs(&local_user_data.user_data);
            if(err != ESP_OK)
                return ERROR;
        break;
        case NEXT_SONG_CMD: // Siguiente cancion
            #if DEBUG    
            printf("Comando Next Song\n");
            #endif
            esp_avrc_ct_send_passthrough_cmd(0, ESP_AVRC_PT_CMD_FORWARD, ESP_AVRC_PT_CMD_STATE_PRESSED);
            esp_avrc_ct_send_passthrough_cmd(0, ESP_AVRC_PT_CMD_FORWARD, ESP_AVRC_PT_CMD_STATE_RELEASED);
               
        break;
        case PREVIOUS_SONG_CMD: // Anterior cancion
            #if DEBUG    
            printf("Comando Previous Song\n");
            #endif
            esp_avrc_ct_send_passthrough_cmd(0, ESP_AVRC_PT_CMD_BACKWARD, ESP_AVRC_PT_CMD_STATE_PRESSED);
            esp_avrc_ct_send_passthrough_cmd(0, ESP_AVRC_PT_CMD_BACKWARD, ESP_AVRC_PT_CMD_STATE_RELEASED);
              
        break;
        case STOP_SONG_CMD: // Pausar cancion
            #if DEBUG    
            printf("Comando STOP Song\n");
            #endif
            static uint8_t fStop = false;
            if(fStop) {
                esp_avrc_ct_send_passthrough_cmd(0, ESP_AVRC_PT_CMD_PLAY, ESP_AVRC_PT_CMD_STATE_PRESSED);
                esp_avrc_ct_send_passthrough_cmd(0, ESP_AVRC_PT_CMD_PLAY, ESP_AVRC_PT_CMD_STATE_RELEASED);
            } else {
                esp_avrc_ct_send_passthrough_cmd(0, ESP_AVRC_PT_CMD_PAUSE, ESP_AVRC_PT_CMD_STATE_PRESSED);
                esp_avrc_ct_send_passthrough_cmd(0, ESP_AVRC_PT_CMD_PAUSE, ESP_AVRC_PT_CMD_STATE_RELEASED);
            }
            fStop = !fStop;  
        break;
        case LOUD_CONFIG_CMD:
            
            vTaskDelay(pdMS_TO_TICKS(100)); // Espero 300 ms para recibir todo.

            #if DEBUG    
            printf("Comando LOUDNESS\n");
            #endif
            err = i2c_slave_read_buffer(I2C_SLAVE_NUM, buff_rx,2, pdMS_TO_TICKS(100));

            if(err == 0)
                return ERROR;
            
            #if DEBUG    
            printf("llego en loud  %i - %i\n",buff_rx[0],buff_rx[1]);
            #endif
            if(buff_rx[0] == 0 || buff_rx[0] == 1) // chequeo que el dato sea valido
                correccion_loudness = buff_rx[0] ;
            else
                return ERROR;
            if(buff_rx[1] <= 100){
                loudness_target =20 * log10((buff_rx[1]* (MAX_LOUDNESS-MIN_LOUDNESS) / 100) + MIN_LOUDNESS);
            } 
            else{
                return ERROR;
            }
            if(correccion_loudness == 0)
                volume_task_reset();
            #if DEBUG    
            printf("Se seteo el loudness en %f, y se encuentra en un estado %i\n",loudness_target,correccion_loudness);
            #endif
        break;
            case GET_MUSIC_STATE_CMD: // Se pide el nivel de progreso de la cancion
                i2c_reset_tx_fifo(I2C_SLAVE_NUM);
                if(tiempo_total != 0){
                    progreso = (uint8_t) (tiempo_actual*100/tiempo_total);
                }
                else{
                     progreso = 100;
                }

                err = i2c_slave_write_buffer(I2C_SLAVE_NUM,(const uint8_t *)&progreso,1,pdMS_TO_TICKS(100));

                if(err == ESP_FAIL)
                    return ERROR;
        
        break;
        case GET_BT_STATE_CMD: // Se pide el nivel de progreso de la cancion
            i2c_reset_tx_fifo(I2C_SLAVE_NUM);
          
            err = i2c_slave_write_buffer(I2C_SLAVE_NUM,(const uint8_t *)&bt_state_global,1,pdMS_TO_TICKS(100));
            #if DEBUG    
            printf("Estado del bt en: %i\n",bt_state_global);
            #endif
            if(err == ESP_FAIL)
                return ERROR;
    
        break;
        default:
        #if DEBUG    
        printf("Comando Invalido\n");
        #endif  
        return INVALID_COMMAND;
        break;


    }
    return ESP_OK;
}

void print_data(UserData data){
    
    printf("Estructura UserData : \n");
    
    printf("Isinitailized: %i \n",data.Isinitailized);
    
    printf("main_volume  : %i \n",data.main_volume);
    
    printf("general_equalizer: \n");
    printf("\t _20hz    : %i \n",data.general_equalizer._20hz);
    printf("\t _200hz   : %i \n",data.general_equalizer._200hz);
    printf("\t _500hz   : %i \n",data.general_equalizer._500hz);
    printf("\t _1000hz  : %i \n",data.general_equalizer._1000hz);
    printf("\t _4000hz  : %i \n",data.general_equalizer._4000hz);
    printf("\t _8000hz  : %i \n",data.general_equalizer._8000hz);
    printf("\t _16000hz : %i \n",data.general_equalizer._16000hz);
    
    printf("general_equalizer_pers: \n");
    printf("\t _20hz    : %i \n",data.general_equalizer_pers._20hz);
    printf("\t _200hz   : %i \n",data.general_equalizer_pers._200hz);
    printf("\t _500hz   : %i \n",data.general_equalizer_pers._500hz);
    printf("\t _1000hz  : %i \n",data.general_equalizer_pers._1000hz);
    printf("\t _4000hz  : %i \n",data.general_equalizer_pers._4000hz);
    printf("\t _8000hz  : %i \n",data.general_equalizer_pers._8000hz);
    printf("\t _16000hz : %i \n",data.general_equalizer_pers._16000hz);
    
    printf("audio_input: \n");
    printf("\t type_in        : %i \n",data.audio_input.type_in);
    printf("\t channel        : %i \n",data.audio_input.channel);
    printf("\t channel_balance: %i \n",data.audio_input.channel_balance);
    
    printf("audio_output [0] :\n");// paja imprimir todos, solo imprimo el primero
    printf("\t channel        : %i \n",data.audio_output[0].channel);
    printf("\t state          : %i \n",data.audio_output[0].state);
    printf("\t channel_volume : %i \n",data.audio_output[0].channel_volume);
    printf("\t type_equalizer : %i \n",data.audio_output[0].type_equalizer);
    printf("\t channel_audio  : %i \n",data.audio_output[0].channel_audio);

    printf("audio_output_pers [0] :\n"); // paja imprimir todos, solo imprimo el primero
    printf("\t channel        : %i \n",data.audio_output_pers[0].channel);
    printf("\t state          : %i \n",data.audio_output_pers[0].state);
    printf("\t channel_volume : %i \n",data.audio_output_pers[0].channel_volume);
    printf("\t type_equalizer : %i \n",data.audio_output_pers[0].type_equalizer);
    printf("\t channel_audio  : %i \n",data.audio_output_pers[0].channel_audio);

    printf("general_config: \n");
    printf("\t system_type       :%i \n",data.general_config.system_type);
    printf("\t equalization_type :%i \n",data.general_config.equalization_type);
    printf("\t max_volume        :%i \n",data.general_config.max_volume);
    printf("\t loudness_state    :%i \n",data.general_config.loudness_state);

    printf("channel_in_use : %i\n",data.channelInUse);
}

esp_err_t init_nvs(void){
    esp_err_t error;

    error = nvs_flash_init();

    return error;
}

/// @brief Permite leer la estructura de datos 
/// @param key 
/// @param value 
/// @return 
esp_err_t read_nvs(UserData *value){ // tener cuidado aca que no necesariamente sean uint8_t los datos a guardar

    esp_err_t error;
    size_t largo = 64U;
  
    // Open
    error = nvs_open(NAME_SPACE, NVS_READWRITE,&app_nvs_handle);

    if(error != ESP_OK){
        #if DEBUG
        printf("Error abriendo el NVS\n");
        #endif
        return error;
    }
    else{
        #if DEBUG
        printf("Se abrio con exito el NVS\n");
        #endif
    }

    // Read data
    error = nvs_get_blob(app_nvs_handle, key,  value, &largo);

    if(error != ESP_OK){
        #if DEBUG
        printf("Error leyendo en el NVS, largo: %i\n",largo);
        #endif

        return error;
    }
    else{
        #if DEBUG
        printf("Se leyo con exito el NVS\n");
        #endif
    }


    //Close NVS
    nvs_close(app_nvs_handle);
    
    return error;

}

/// @brief Funcion que escribe la estructura UserData en flash
/// @param value Estructura a escribir
/// @return Devuelve ESP_OK si se escribio con exito.
esp_err_t write_nvs(UserData *value){ // tener cuidado aca que no necesariamente sean uint8_t los datos a guardar

    esp_err_t error;

    // Open
    #if DEBUG
    //print_data(*value);
    #endif

    error = nvs_open(NAME_SPACE, NVS_READWRITE,&app_nvs_handle);

    if(error != ESP_OK){
        #if DEBUG
        printf("Error abriendo el NVS\n");
        #endif
        return error;
    }
    else{
        #if DEBUG
        printf("Se abrio con exito el NVS\n");
        #endif
    }

    // Write value 
    error = nvs_set_blob(app_nvs_handle, key , (const void*) value, sizeof(UserData));

    if(error != ESP_OK){
        #if DEBUG
        printf("Error escribiendo en el NVS\n");
        #endif

        return error;
    }
    else{
        #if DEBUG
        printf("Se escribio con exito el NVS\n");
        #endif

    }

    // Commit written value.
    // After setting any values, nvs_commit() must be called to ensure changes are written
    // to flash storage. Implementations may write to storage at other times,
    // but this is not guaranteed.
    error = nvs_commit(app_nvs_handle);
    if (error != ESP_OK) return error;

    //Close NVS
    nvs_close(app_nvs_handle);

    return error;

}


/////////////////////////////////////// MainFunction ////////////////////////////////////////////////
void comi2c_start_up() {
esp_err_t err=i2c_slave_init();
    i2c_set_pin(I2C_SLAVE_NUM, I2C_SLAVE_SDA_IO, I2C_SLAVE_SCL_IO, GPIO_PULLUP_DISABLE, GPIO_PULLUP_DISABLE,    I2C_MODE_SLAVE);

    if(err == ESP_OK){
        #if DEBUG
        printf("Se inicio bien I2C\n");
        #endif
    }

    err = init_nvs();
    if(err == ESP_OK){
        #if DEBUG
        printf("Se inicio bien NVS\n");
        #endif
    }
    
    read_nvs(&local_user_data.user_data);

    //local_user_data.user_data.Isinitailized = 0; // descomentar estas lineas para reiniciar la config
    //write_nvs(&local_user_data.user_data);

    print_data(local_user_data.user_data);

    xTaskCreate(i2c_task, "i2c_task", 1024 * 2, (void *)0, 11, NULL);
        
}
