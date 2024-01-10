#include <gui/ecualizadormenu_screen/EcualizadorMenuView.hpp>


EcualizadorMenuView::EcualizadorMenuView()
{

}

void EcualizadorMenuView::setupScreen()
{
	get_user_data(&buff_data);
    EcualizadorMenuViewBase::setupScreen();
    UpdateGraph();
}

void EcualizadorMenuView::tearDownScreen()
{
	set_user_data(buff_data);
    EcualizadorMenuViewBase::tearDownScreen();
}


void EcualizadorMenuView::UpdateGraph(){

	uint8_t muestras = this->GraphEcualizador.getMaxCapacity(); // obtengo cantidad de puntos que necesito calcular

	// Parametros del grafico

	uint32_t max_y = this->GraphEcualizador.getGraphRangeYMaxAsInt();
	uint32_t min_y = this->GraphEcualizador.getGraphRangeYMinAsInt();

	// Armo puntos conocidos para metodo lagranje
	int8_t puntos_conocidos[CANT_FREQ_FILTRO];
	uint32_t freq_conocidas[CANT_FREQ_FILTRO] = {20,200,500,1000,4000,8000,16000};

	// Que filtro tengo que armar:
	if(buff_data.general_config.equalization_type == EQ_PROF_PERS){
		puntos_conocidos[0] = buff_data.general_equalizer_pers._20hz;
		puntos_conocidos[1] = buff_data.general_equalizer_pers._200hz;
		puntos_conocidos[2] = buff_data.general_equalizer_pers._500hz;
		puntos_conocidos[3] = buff_data.general_equalizer_pers._1000hz;
		puntos_conocidos[4] = buff_data.general_equalizer_pers._4000hz;
		puntos_conocidos[5] = buff_data.general_equalizer_pers._8000hz;
		puntos_conocidos[6] = buff_data.general_equalizer_pers._16000hz;
	}
	else{
		puntos_conocidos[0] = buff_data.general_equalizer._20hz;
		puntos_conocidos[1] = buff_data.general_equalizer._200hz;
		puntos_conocidos[2] = buff_data.general_equalizer._500hz;
		puntos_conocidos[3] = buff_data.general_equalizer._1000hz;
		puntos_conocidos[4] = buff_data.general_equalizer._4000hz;
		puntos_conocidos[5] = buff_data.general_equalizer._8000hz;
		puntos_conocidos[6] = buff_data.general_equalizer._16000hz;
	}
	// Defino vector de puntos conocidos
	Punto_t valores[CANT_FREQ_FILTRO];

	for(int i=0;i<CANT_FREQ_FILTRO;i++){
		valores[i].y = puntos_conocidos[i];
		valores[i].x = freq_conocidas[i];
	}

	// reservo la memoria para calcular esos puntos
	Punto_t valor_calculados[muestras];

	//consigo los valores de cada punto

	calcularPuntos(valores,CANT_FREQ_FILTRO,muestras,valor_calculados);

	float y=0;
	float x=0;

	for(uint8_t i=0 ; i < muestras ;i++){

		y = valor_calculados[i].y + max_y/2;
		x= valor_calculados[i].x;
		if (y >= max_y) y = max_y ;
		if (y <= min_y) y = min_y ;

		this->GraphEcualizador.addDataPoint(x,y);
	}

}

