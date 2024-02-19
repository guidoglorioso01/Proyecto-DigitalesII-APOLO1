#include "volume_control.h"
#include "esp_log.h"
// #include "driver/adc.h"
#include "driver/uart.h"

const float32_t Coeff_firs_stage[] = {1.53051345, -2.6473075,  1.1656303, -1.66132685, 0.71016304}; 
const float32_t Coeff_second_stage[] = {0.99443571, -1.98887142,  0.99443571, -1.98885527, 0.98888757};

FloatQueue sub_windows[NUM_CHANNELS]; 
QueueHandle_t queue_data_out_izq;
QueueHandle_t queue_data_out_der;
QueueHandle_t queue_correccion;
SemaphoreHandle_t sem_buff_full;
float coeffs_lpf[5];

uint8_t correccion_loudness = 0;
float32_t loudness_target = 60;
TaskHandle_t handler_task_loudness;
float ganancia = 0;
//the following code changes the volume
uint8_t *volume_control_changeVolume(const uint8_t *data, uint8_t *outputData, size_t size, float volume) {
    const int numBytesShifted = 2;

     int16_t pcmData;
    static float32_t correccion=1;
    float32_t dato_recibido;
    float32_t buff;
    int channel = 0;
    
    memcpy(outputData, data, size);

    size_t h = 0;

    if(correccion_loudness == 1){
        if(xQueueReceive(queue_correccion,&dato_recibido,0) == pdPASS ){ // Pude retirar un dato de la queue
            correccion = dato_recibido;
            //printf("Correccion: %f\n",correccion);
        }
        for (h = 0; h < size; h += numBytesShifted) {

            pcmData = ((uint16_t) data[h +1 ] << 8) | data[h];
            buff =(float) (pcmData * correccion * volume); // correccion
            pcmData =(int16_t) buff;
            outputData[h+1] = pcmData >> 8;
            outputData[h] = pcmData;
            #ifdef STEREO
            if(channel){                
                xQueueSend(queue_data_out_izq,&pcmData,0);
            }
            else{
            xQueueSend(queue_data_out_der,&pcmData,0);
            }
            #else
            if(channel){                
                xQueueSend(queue_data_out_izq,&pcmData,0);
            }
            #endif
        
            
            channel = !channel;
        }
    }
    else{

        for (h = 0; h < size; h += numBytesShifted) {
            pcmData = ((uint16_t) data[h +1 ] << 8) | data[h];
            buff =(float) (pcmData * 1 * volume); // correccion
            pcmData =(int16_t) buff;
            outputData[h+1] = pcmData >> 8;
            outputData[h] = pcmData;
        }
    }
   
   
    return outputData;
}


void volume_task_reset(){
    
    for(uint32_t c=0;c < NUM_CHANNELS;c++){
        clearQueue(&sub_windows[c]);
    }

    ganancia = 0;
}

void volume_task_start_up(){

    // Queue para enviar comunicar los datos recibidos entre TASK
    queue_data_out_izq = xQueueCreate(SUB_WINDOW_LEN*NUM_CHANNELS,sizeof(int16_t)); 
    
    #ifdef STEREO
    queue_data_out_der = xQueueCreate(SUB_WINDOW_LEN*NUM_CHANNELS,sizeof(int16_t));
    #endif

    // Queue para enviar la correccion que se debe hacer entre TASK
    queue_correccion = xQueueCreate(10,sizeof(float32_t));

    for (uint32_t i = 0; i < NUM_CHANNELS; i++)
    {
        initQueue(&sub_windows[i]);
    }
    
    //Creo la tarea que calcula el loudnes
    xTaskCreate(volume_task_handler, "VolTask", 3000*NUM_CHANNELS, NULL, tskIDLE_PRIORITY + 1 , NULL); // Una prioridad menos que la de BT
}


float32_t abs_float(float32_t num){
    if(num < 0)
        return -num;
    return num;
}



void volume_task_handler(void *arg){
    float32_t loudness_relativo;
    //static float32_t ganancia=0;
    float32_t correccion;
    uint16_t K =5;
    uint16_t h = 3;
    float32_t loudness =0;

    while(1){
        if(correccion_loudness == 1){
            if(loudness_algoritmo(loudness_target, &loudness_relativo)==1){ 
                ESP_LOGI(LOUD_TAG, "Loud: %.3f ; ganancia: %.3f ; Loud_target: %.3f\n",loudness_relativo,ganancia,loudness_target);
                //printf("Loud: %.3f ; ganancia: %.3f ; Loud_target: %.3f\n",loudness_relativo,ganancia,loudness_target);
                //printf("%.3f,",loudness_relativo);  


                if((abs_float(loudness_relativo) > h) && ((loudness_relativo + loudness_target )  > 10)){
                    ganancia += ((-loudness_relativo)/K); // La ganancia esta en dB
                    correccion = pow(10,(ganancia/20));
                    xQueueSend(queue_correccion,&correccion,0);
                }
                    
            }
        correccion = 0;
        
        xQueueReset(queue_data_out_izq); // Borro todo lo que se haya almacenado      
        //vTaskDelay(pdMS_TO_TICKS(20));
        }
        else{
            vTaskDelay(pdMS_TO_TICKS(20));
        }

    }

}

// // Control de loudnes Algorithm

// TODO: HACER FUNCIONES DE FILTRADO 
/**
 ** @brief 
 * Esta funcion realiza un filtrado IIR, para ponderar los efectos acusticos en la cabeza y los pesos de frecuencia segun
 * el oido humano
 * @param data_in Vector de entrada de datos
 * @param data_out Vector de salida de datos
 * @param len_data Cantidad de datos en data_in
 */
uint8_t filter_data(QueueHandle_t queue_datos,float32_t* data_out){
    
    int16_t data;

    float32_t *buff = (float32_t *)malloc(SUB_WINDOW_LEN * sizeof(float32_t));
        
    if(buff==NULL){
        ESP_LOGE(LOUD_TAG, "error");
        return 0;
    }
    
    float w[5] = {0,0};

    for(int i=0;i<SUB_WINDOW_LEN;i++){  
        xQueueReceive(queue_datos,&data,portMAX_DELAY);
  
        data_out[i] = data;
    }

    dsps_biquad_f32_ae32((float*)data_out,buff, SUB_WINDOW_LEN,(float*) Coeff_firs_stage, w);
    dsps_biquad_f32_ae32((float*)buff,data_out, SUB_WINDOW_LEN,(float*) Coeff_second_stage, w);
    
    free(buff);
    return 1;
}

/**
 * @brief 
 * Esta funcion recibe un buffer de time_gate segundos muestreados a una Fs frecuencia, y un loudness objetivo (loudness target).
 * Calcula el loudness de la señal teniendo en cuenta un buff_previous_data y la señal dada en buff_gate, escribiendo en la variable
 * correction la correccion necesaria de loudness para que coincida con el valor objetivo. 
 * En caso de exito devuelve 1, en caso de que no se haya llenado la cola devuelve 2, en caso de fallo devuelve 0.
 * @param buff_gate 
 * @param target_loundness 
 * @param time_gate 
 * @param fs Frecuencia de muestreo
 * @return uint8_t 

 * 
 * @param buff_gate Buffer con datos de entrada
 * @param target_loundness Loudness que se desea alcanzar
 * @param time_gate Tiempo de cada "Gate" [0.05 - 0.1 - 0.2 - 0.3 - 0.4]
 * @param fs Frecuencia de muestreo
 * @param correction Loudness a corregir
 * @return uint8_t 
 */
uint8_t loudness_algoritmo(float32_t target_loundness,float32_t *correction){

    // Los vecotres se reservan con malloc ya que haciendolo de forma estatica tira hardfault.
    static uint8_t entradas = 0;

    float32_t **buff_filtered = (float32_t **)malloc( NUM_CHANNELS * sizeof(float32_t));
    
    if(buff_filtered==NULL){
        ESP_LOGE(LOUD_TAG, "error malloc 1");
        return 7;
    }

    for (int i = 0; i < NUM_CHANNELS; i++) {
        buff_filtered[i] = (float32_t *)malloc(SUB_WINDOW_LEN * sizeof(float32_t));
            
        if(buff_filtered[i]==NULL){
            for(int i2=0;i2<i;i2++){ // Agrego
                free(buff_filtered[i2]);
            }
            free(buff_filtered); //
            ESP_LOGE(LOUD_TAG, "error malloc 2");
            return 7;
        }
        
    }
    uint8_t state_filter;
    // Realizo filtrados a señal de entrada
    for(int i=0;i < NUM_CHANNELS;i++){
        #ifdef STEREO

        if(i == 0)
            state_filter = filter_data(queue_data_out_izq,buff_filtered[i]);    
        else
            state_filter = filter_data(queue_data_out_der,buff_filtered[i]);

        #else

        state_filter = filter_data(queue_data_out_izq,buff_filtered[i]);

        #endif

        if(!state_filter){
            for(int i2=0;i2<NUM_CHANNELS;i2++){ // Agrego
                free(buff_filtered[i2]);
            }
            free(buff_filtered); //
            return 7;
        }
    }

    // Obtengo las subventanas de las muestras (Tg) pasadas en la funcion
    
    for(uint32_t c=0;c < NUM_CHANNELS;c++){
        double subWindow=0;
        // Proceso los datos de la SubWindow ingresada
        for (int i = 0; i < SUB_WINDOW_LEN; i++) {
            subWindow += buff_filtered[c][i]*buff_filtered[c][i];
        }
       
        //Guardo el dato calculado
        enqueue(&sub_windows[c],(float)subWindow);
    }

    //LIBERO BUFF_FILTRADO
    for(int i=0;i<NUM_CHANNELS;i++){
        free(buff_filtered[i]);
    }
    free(buff_filtered);

    // Si la cola no se lleno por completo NO calculo loudness (4 segundo de audio)
    
    if( !isQueueFull(&sub_windows[0]) ){ // con comprobar una es suficiente
        return 2;
    }

    // Hasta que no recibo un string de 0.4seg no calculo el loudness (es decir se debe llamar CANT_SUB_WINDOWS veces a la funcion para calcular un loudness)
    entradas++;
    if( entradas < CANT_SUB_WINDOWS){ // con comprobar una es suficiente
        return 3;
    }
    entradas = 0;

    // Calculo la potencia de cada ventana
    
    float32_t **buff_windows = (float32_t **)malloc( NUM_CHANNELS * sizeof(float32_t));
    
    if(buff_windows==NULL){
        ESP_LOGE(LOUD_TAG, "error malloc 3");
        return 7;
    }
    
    for (int i = 0; i < NUM_CHANNELS; i++) {
        buff_windows[i] = (float32_t *)malloc(MAX_QUEUE_SIZE * sizeof(float32_t));
        
        if(buff_windows[i]==NULL){
            for(int i2=0;i2<i;i2++){ // Agrego
                free(buff_windows[i2]);
            }
            free(buff_windows); //
            ESP_LOGE(LOUD_TAG, "error malloc 4");
            return 7;
        }   
    }

    float32_t **z_ij = (float32_t **)malloc( NUM_CHANNELS * sizeof(float32_t));
    
    if(z_ij==NULL){
        for(int i2=0;i2<NUM_CHANNELS;i2++){ // Agrego
            free(buff_windows[i2]);
        }
        free(buff_windows); //
        ESP_LOGE(LOUD_TAG, "error malloc 5");
        return 7;
    }
    
    for (int i = 0; i < NUM_CHANNELS; i++) {
        z_ij[i] = (float32_t *)malloc(NUMBER_OF_WINDOWS * sizeof(float32_t));
             
        if(z_ij[i]==NULL){
            for(int i2=0;i2<i;i2++){ // Agrego
                free(z_ij[i2]);
            }
            free(z_ij); //

            for(int i2=0;i2<NUM_CHANNELS;i2++){ // Agrego
                free(buff_windows[i2]);
            }
            free(buff_windows); //
            ESP_LOGE(LOUD_TAG, "error malloc 6");
            return 7;
        }
        
    }

    for(int i=0;i<NUM_CHANNELS;i++)
        copyQueueToVector(&sub_windows[i],buff_windows[i]);

    double sum = 0.0;
    for(uint32_t i=0;i < NUM_CHANNELS;i++){
        for (uint32_t j = 0; j < NUMBER_OF_WINDOWS; j++) {
            sum = 0;
            for (uint32_t k = (j*OFFSET_J); k < (j * OFFSET_J + CANT_SUB_WINDOWS); k++) {
                sum += buff_windows[i][k];
            }
            z_ij[i][j] =(float32_t) (sum * (float32_t)(1.0/( WINDOW_TIME * FS)));
        }
    }

    //LIBERO BUFF_WINDOWS
    for(int i=0;i<NUM_CHANNELS;i++){
        free(buff_windows[i]);
    }
    free(buff_windows);

    // Cálculo del loudness de cada ventana

    float *loudness_j = (float *)malloc( NUMBER_OF_WINDOWS * sizeof(float));
  
    if(loudness_j==NULL){
        for(int i2=0;i2<NUM_CHANNELS;i2++){ // Agrego
            free(z_ij[i2]);
        }
        free(z_ij); //
        ESP_LOGE(LOUD_TAG, "error malloc 7");
        return 7;
    }

    float coef_canales[5] = {1,1,1,1.41,1.41};
    for(uint32_t j = 0; j < NUMBER_OF_WINDOWS; j++) {
        double suma_z_ij = 0;
        for(uint32_t c=0;c < NUM_CHANNELS;c++){
            suma_z_ij += z_ij[c][j] * coef_canales[c];
        }
      
        if(suma_z_ij > 0)
            loudness_j[j] = -0.691 + 10 * log10(suma_z_ij);
        else
            loudness_j[j] = -1000; //Le pongo como que es practicamente cero el loudnes pero en dB
    }

    // Calculo el loudness absoluto

    double loudness_abs = 0.0;
    uint32_t len = 0;
    for (uint32_t i = 0; i < NUMBER_OF_WINDOWS; i++) {
        if(loudness_j[i] > GAMA_A){
            double suma_z_ij = 0;
            for(uint32_t c=0;c < NUM_CHANNELS;c++){
                suma_z_ij += z_ij[c][i] * coef_canales[c];
            }
            loudness_abs += suma_z_ij;
            len++;
        }
    }
    loudness_abs = -0.691 + 10* log10(loudness_abs/(len));
    
    // Calculo coeficiente para el calculo de loudness relativo
    double gama_b = loudness_abs - 10;

    loudness_abs = 0.0;
    len = 0;
    for (uint32_t i = 0; i < NUMBER_OF_WINDOWS; i++) {
        if((loudness_j[i] > gama_b)&&(loudness_j[i] > GAMA_A)){
            double suma_z_ij = 0;
            for(uint32_t c=0;c < NUM_CHANNELS;c++){
                suma_z_ij += z_ij[c][i] * coef_canales[c];
            }
            loudness_abs += suma_z_ij;
            len++;
        }
    }

    //LIBERO Z_IJ
    for(int i=0;i<NUM_CHANNELS;i++){
        free(z_ij[i]);
    }
    free(z_ij);
    free(loudness_j);

    loudness_abs = 10* log10(loudness_abs/len);
    loudness_abs = -0.691 + loudness_abs;

    *correction = loudness_abs - target_loundness;
    
    return 1;
    
}



