#include <gui/crossovermenu_screen/CrossOverMenuView.hpp>

extern SemaphoreHandle_t semCalcCoefs;

CrossOverMenuView::CrossOverMenuView()
{

}

void CrossOverMenuView::setupScreen()
{
	get_user_data(&buff_data);
    CrossOverMenuViewBase::setupScreen();
    UpdateBut();
    UpdateGraph();
 }

void CrossOverMenuView::tearDownScreen()
{

	set_user_data(buff_data);
	if(buff_data.general_config.system_type ==SYSTEM_PERS)
		xSemaphoreGive(semCalcCoefs);
    CrossOverMenuViewBase::tearDownScreen();
}

void CrossOverMenuView::GFXeditCrossOver()
{
	if(buff_data.general_config.system_type == SYSTEM_PERS){
		switch(buff_data.audio_output[buff_data.channelInUse].type_equalizer){
			default:
			case TYPE_FLAT:
				buff_data.audio_output[buff_data.channelInUse].type_equalizer = TYPE_LP1;
			break;
			case TYPE_LP1:
				buff_data.audio_output[buff_data.channelInUse].type_equalizer = TYPE_LP2;
				break;
			case TYPE_LP2:
				buff_data.audio_output[buff_data.channelInUse].type_equalizer = TYPE_BP1;
				break;
			case TYPE_BP1:
				buff_data.audio_output[buff_data.channelInUse].type_equalizer = TYPE_BP2;
				break;
			case TYPE_BP2:
				buff_data.audio_output[buff_data.channelInUse].type_equalizer = TYPE_HP1;
				break;
			case TYPE_HP1:
				buff_data.audio_output[buff_data.channelInUse].type_equalizer = TYPE_HP2;
				break;
			case TYPE_HP2:
				buff_data.audio_output[buff_data.channelInUse].type_equalizer = TYPE_FLAT;
				break;
		}
		buff_data.audio_output_pers[buff_data.channelInUse].type_equalizer =buff_data.audio_output[buff_data.channelInUse].type_equalizer;
	}
	UpdateBut();
	UpdateGraph();
}

void CrossOverMenuView::UpdateBut(){

	switch(buff_data.audio_output[buff_data.channelInUse].type_equalizer){
		default:
		case TYPE_FLAT:
			this->ButtEdit.setLabelText(touchgfx::TypedText(T_SIN_FILTRO_CROSSOVER));
		break;
		case TYPE_LP1:
			this->ButtEdit.setLabelText(touchgfx::TypedText(T_PASA_BAJOS1_CROSSOVER));
		break;
		case TYPE_LP2:
			this->ButtEdit.setLabelText(touchgfx::TypedText(T_PASA_BAJOS2_CROSSOVER));
		break;
		case TYPE_BP1:
			this->ButtEdit.setLabelText(touchgfx::TypedText(T_PASA_BANDAS1_CROSSOVER));
		break;
		case TYPE_BP2:
			this->ButtEdit.setLabelText(touchgfx::TypedText(T_PASA_BANDAS2_CROSSOVER));
		break;
		case TYPE_HP1:
			this->ButtEdit.setLabelText(touchgfx::TypedText(T_PASA_ALTOS1_CROSSOVER));
		break;
		case TYPE_HP2:
			this->ButtEdit.setLabelText(touchgfx::TypedText(T_PASA_ALTOS2_CROSSOVER));
		break;
	}

}

void CrossOverMenuView::UpdateGraph(){

	uint8_t muestras = this->GraphCrossOver.getMaxCapacity(); // obtengo cantidad de puntos que necesito calcular

	// Parametros del grafico

	uint32_t max_y = this->GraphCrossOver.getGraphRangeYMaxAsInt();
	uint32_t min_y = this->GraphCrossOver.getGraphRangeYMinAsInt();

	// Armo puntos conocidos para metodo lagranje
	uint8_t puntos_conocidos[CANT_FREQ_FILTRO];
	uint32_t freq_conocidas[CANT_FREQ_FILTRO] = {20,200,500,1000,4000,8000,16000};

	// Que filtro tengo que armar:
	switch(buff_data.audio_output[buff_data.channelInUse].type_equalizer){
	default:
	case TYPE_FLAT:
		CROSSOVER_FLAT(puntos_conocidos);
	break;
	case TYPE_LP1:
		CROSSOVER_LP1(puntos_conocidos);
		break;
	case TYPE_LP2:
		CROSSOVER_LP2(puntos_conocidos);
		break;
	case TYPE_BP1:
		CROSSOVER_BP1(puntos_conocidos);
		break;
	case TYPE_BP2:
		CROSSOVER_BP2(puntos_conocidos);
		break;
	case TYPE_HP1:
		CROSSOVER_HP1(puntos_conocidos);
		break;
	case TYPE_HP2:
		CROSSOVER_HP2(puntos_conocidos);
		break;
	}

	// Defino vector de puntos conocidos
	Punto_t valores[CANT_FREQ_FILTRO];

	for(int i=0;i<CANT_FREQ_FILTRO;i++){
		valores[i].y = -puntos_conocidos[i];
		valores[i].x = freq_conocidas[i];
	}

	// reservo la memoria para calcular esos puntos
	Punto_t valor_calculados[muestras];

	//consigo los valores de cada punto

	calcularPuntos(valores,CANT_FREQ_FILTRO,muestras,valor_calculados);

	float y=0;
	float x=0;

	for(uint8_t i=0 ; i < muestras ;i++){
		/*
		 * Aca hay que poner lo que calcula el grafico
		 * */
		y = valor_calculados[i].y + max_y;
		x= valor_calculados[i].x;
		if (y >= max_y) y = max_y ;
		if (y <= min_y) y = min_y ;

		this->GraphCrossOver.addDataPoint(x, y);
	}

}
