#if defined _marp_concesionaria_included
	#endinput
#endif
#define _marp_concesionaria_included

//=================================CONSTANTES===================================

#define MAX_DEALERSHIPS         7
#define MAX_DEALERSHIP_VEHICLES 30

#define DEALERSHIP_GROTTI       0
#define DEALERSHIP_JEFFERSON    1
#define DEALERSHIP_PINAR     	2
#define DEALERSHIP_CIUDADMOTO   3
#define DEALERSHIP_FREDDI     	4
#define DEALERSHIP_GABOTT     	5
#define DEALERSHIP_HAUL      	6

#define MODEL_CREATE            1
#define MODEL_DELETE            2

#define DEALERSHIP_PAYMENT_CASH 1
#define DEALERSHIP_PAYMENT_BANK 2

//=================================VARIABLES====================================

enum DealershipInfo {
	dName[32],
	Float:dX, // Donde est� la concesionaria y su pickup
	Float:dY,
	Float:dZ,
	Float:dParkingX, // Donde va a aparecer el auto estacionado una vez comprado
	Float:dParkingY,
	Float:dParkingZ,
	Float:dParkingAngle
}
static const Dealership[MAX_DEALERSHIPS][DealershipInfo] = {
	{"Concesionarios Grotti", 542.61, -1298.90, 17.24, 546.86, -1266.70, 16.86, 302.50},
	{"Concesionarios Jefferson", 2131.77, -1150.91, 24.02, 2128.34, -1135.81, 25.16, 355.38},
	{"Pinar Automotores", 986.23, -1116.85, 27.28, 980.71, -1135.76, 23.46, 90.74},
	{"Concesionaria Ciudad Moto", 1300.74, -1875.69, 13.55, 1299.64, -1866.97, 13.11, 28.43},
	{"Freddi Aviones", 1921.14, -2235.02, 13.54, 1903.13, -2315.69, 13.18, 218.39},
	{"Nautica Gabott", 2939.37, -2051.45, 3.54, 2937.79, -2064.40, 0.09, 271.84},
	{"RS Haul Utilitarios", 2248.24, -2185.43, 13.57, 2219.23, -2178.56, 14.04, 45.25}
};

new DealershipVehicles[MAX_DEALERSHIPS][MAX_DEALERSHIP_VEHICLES];

new PlayerText:PTD_Concesionaria[MAX_PLAYERS][2];

new carModelSelection[MAX_PLAYERS],
	carColor1Selection[MAX_PLAYERS],
	carColor2Selection[MAX_PLAYERS],
	carPaymentSelection[MAX_PLAYERS];

//============================FUNCIONES PUBLICAS================================

GetNearestDealership(playerid, Float:distance)
{
	for(new i = 0; i < MAX_DEALERSHIPS; i++)
	{
	    if(PlayerToPoint(distance, playerid, Dealership[i][dX], Dealership[i][dY], Dealership[i][dZ]))
	        return i;
	}
	return -1;
}

Dealership_LoadPickups()
{
	new str[128];

	for(new i = 0; i < MAX_DEALERSHIPS; i++)
	{
		CreateDynamicPickup(1239, 1, Dealership[i][dX], Dealership[i][dY], Dealership[i][dZ]);
		format(str, sizeof(str), "%s\nEscribe '/vercatalogo' para ver los veh�culos en venta.", Dealership[i][dName]);
    	CreateDynamic3DTextLabel(str, COLOR_WHITE, Dealership[i][dX], Dealership[i][dY], Dealership[i][dZ] + 0.75, 20.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1);
 	}
}

Dealerships_LoadData()
{
	new query[128];
	
	for(new i = 0; i < MAX_DEALERSHIPS; i++)
	{
		format(query, sizeof(query), "SELECT * FROM `dealerships_info` WHERE `Id`=%d LIMIT %d", i, MAX_DEALERSHIP_VEHICLES);
		mysql_function_query(dbHandle, query, true, "OnDealershipDataLoad", "i", i);
	}
	return 1;
}

forward OnDealershipDataLoad(dealership);
public OnDealershipDataLoad(dealership)
{
   	new rows,
	   	fields,
   		aux = 0;

	cache_get_data(rows, fields);

	if(rows)
	{
		while(aux < rows)
	    {
			DealershipVehicles[dealership][aux] = cache_get_field_content_int(aux, "Model");
			aux ++;
		}
	}
	return 1;
}

forward Dealership_SaveModel(dealership, dealershipSlot, option);
public Dealership_SaveModel(dealership, dealershipSlot, option)
{
	new query[256];

	if(dontsave)
	    return 1;

	if(option == MODEL_CREATE)
	{
		format(query, sizeof(query), "INSERT INTO `dealerships_info` (`Id`, `Model`) VALUES (%d, %d)",
		    dealership,
		    DealershipVehicles[dealership][dealershipSlot]
		);
	}
	else if(option == MODEL_DELETE)
	{
		format(query, sizeof(query), "DELETE FROM `dealerships_info` WHERE `Id`=%d AND `Model`=%d",
  		    dealership,
		    DealershipVehicles[dealership][dealershipSlot]
		);
	}
	
	mysql_function_query(dbHandle, query, false, "", "");
	return 1;
}


Dealership_SearchCar(dealership, modelid)
{
	if(dealership >= 0 && dealership < MAX_DEALERSHIPS)
	{
		for(new i = 0; i < MAX_DEALERSHIP_VEHICLES; i++)
		{
		    if(DealershipVehicles[dealership][i] == modelid)
		        return i;
		}
	}
	return -1;
}

CMD:cardealerdebug(playerid, params[])
{
	new dealership;

	if(sscanf(params, "i", dealership))
	    return SendClientMessage(playerid, COLOR_LIGHTYELLOW2, "{5CCAF1}[Sintaxis]:{C8C8C8} /cardealerdebug [ID concesionaria]");
	if(dealership < 0 || dealership >= MAX_DEALERSHIPS)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "ID de concesionaria inv�lida.");

    for(new x = 0; x < MAX_DEALERSHIP_VEHICLES; x++)
    {
        SendFMessage(playerid, COLOR_YELLOW, "DealershipVehicles[%d][%d] = %d", dealership, x, DealershipVehicles[dealership][x]);
   	}
	return 1;
}

CMD:playercardealerdebug(playerid, params[])
{
	new targetid;

	if(sscanf(params, "i", targetid))
	    return SendClientMessage(playerid, COLOR_LIGHTYELLOW2, "{5CCAF1}[Sintaxis]:{C8C8C8} /playercardealerdebug [playerid]");
	if(targetid < 0 || targetid >= MAX_PLAYERS)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "Jugador inv�lido.");

	SendFMessage(playerid, COLOR_YELLOW, "carModelSelection [%d] = %d", targetid, carModelSelection[targetid]);
	SendFMessage(playerid, COLOR_YELLOW, "carColor1Selection [%d] = %d", targetid, carColor1Selection[targetid]);
	SendFMessage(playerid, COLOR_YELLOW, "carColor2Selection [%d] = %d", targetid, carColor2Selection[targetid]);
	return 1;
}

CMD:agregarmodelo(playerid, params[])
{
	new dealership, modelid;

	if(sscanf(params, "ii", dealership, modelid))
	{
		SendClientMessage(playerid, COLOR_YELLOW2, "{5CCAF1}[Sintaxis]:{C8C8C8} /agregarmodelo [ID concesionaria] [modelo]");
		SendClientMessage(playerid, COLOR_YELLOW2, "{5CCAF1}[INFO]{C8C8C8} IDS de concesionarias: {ffffff}0.Grotti | 1.Jefferson | 2.Pinar | 3.Ciudad moto | 4.Freddi | 5.Gabott | 6.RS Haul");
		return 1;
	}
	if(dealership < 0 || dealership >= MAX_DEALERSHIPS)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "ID de concesionaria inv�lida.");
	if(modelid < 400 || modelid > 611)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "ID de modelo inv�lida.");
	if(GetNearestDealership(playerid, 1.0) != dealership)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "Para usar este comando debes estar en la concesionaria.");

	for(new i = 0; i < MAX_DEALERSHIP_VEHICLES; i++)
	{
	    if(DealershipVehicles[dealership][i] == modelid)
	    {
	        SendFMessage(playerid, COLOR_YELLOW2, "El modelo %d (%s) ya se encuentra a la venta en la concesionaria ID %d (%s).", modelid, GetVehicleModelName(modelid), dealership, Dealership[dealership][dName]);
            return 1;
		}
	}
	
	for(new x = 0; x < MAX_DEALERSHIP_VEHICLES; x++)
	{
	    if(DealershipVehicles[dealership][x] == 0)
	    {
	        DealershipVehicles[dealership][x] = modelid;
	    	SendFMessage(playerid, COLOR_WHITE, "Has agregado el modelo %d (%s) en la concesionaria ID %d (%s).", modelid, GetVehicleModelName(modelid), dealership, Dealership[dealership][dName]);
            Dealership_SaveModel(dealership, x, MODEL_CREATE);
			return 1;
		}
	}
	
	SendFMessage(playerid, COLOR_YELLOW2, "No hay m�s espacio (MAX:%d) para otro modelo a la venta en la consecionaria ID %d (%s).", MAX_DEALERSHIP_VEHICLES, dealership, Dealership[dealership][dName]);
	return 1;
}

CMD:borrarmodelo(playerid, params[])
{
	new dealership, modelid;

	if(sscanf(params, "ii", dealership, modelid))
	{
		SendClientMessage(playerid, COLOR_YELLOW2, "{5CCAF1}[Sintaxis]:{C8C8C8} /borrarmodelo [ID concesionaria] [ID del modelo]");
		SendClientMessage(playerid, COLOR_YELLOW2, "{5CCAF1}[INFO]{C8C8C8} IDS de concesionarias: {ffffff}0.Grotti | 1.Jefferson | 2.Pinar | 3.Ciudad moto | 4.Freddi | 5.Gabott | 6.RS Haul");
		return 1;
	}
	
	if(dealership < 0 || dealership >= MAX_DEALERSHIPS)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "ID de concesionaria inv�lida.");
	if(modelid < 400 || modelid > 611)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "ID de modelo inv�lida.");
	if(GetNearestDealership(playerid, 1.0) != dealership)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "Para usar este comando debes estar en la concesionaria.");

	for(new i = 0; i < MAX_DEALERSHIP_VEHICLES; i++)
	{
	    if(DealershipVehicles[dealership][i] == modelid)
	    {
	        Dealership_SaveModel(dealership, i, MODEL_DELETE);
            DealershipVehicles[dealership][i] = 0;
	        SendFMessage(playerid, COLOR_WHITE, "El modelo %d (%s) ha sido quitado de venta de la concesionaria ID %d (%s).", modelid, GetVehicleModelName(modelid), dealership, Dealership[dealership][dName]);
		    return 1;
		}
	}

	SendFMessage(playerid, COLOR_YELLOW2, "[Error]: el modelo %d (%s) no se encuentra a la venta en la concesionaria ID %d (%s).", modelid, GetVehicleModelName(modelid), dealership, Dealership[dealership][dName]);
	return 1;
}

CMD:vercatalogo(playerid, params[])
{
	new string[2000] = "Modelo\tPrecio\tMaletero",
	    dealership = GetNearestDealership(playerid, 1.0),
	    modelid;

	if(dealership == -1)
     	return SendClientMessage(playerid, COLOR_YELLOW2, "Para usar este comando debes estar en una concesionaria.");
     	
	for(new i = 0; i < MAX_DEALERSHIP_VEHICLES; i++)
	{
	    modelid = DealershipVehicles[dealership][i];
	    if(modelid != 0)
	    {
            if(GetVehicleModelTrunkSpace(modelid) > 0)
            {
        		format(string, sizeof(string), "%s\n%s\t$%d\t%du", string, GetVehicleModelName(modelid), GetVehicleModelPrice(modelid, 100), GetVehicleModelTrunkSpace(modelid));
			}
			else
			{
			    format(string, sizeof(string), "%s\n%s\t$%d\tNo", string, GetVehicleModelName(modelid), GetVehicleModelPrice(modelid, 100));
			}
		}
	}
	Dialog_Show(playerid, Car_Dealer_Step_1, DIALOG_STYLE_TABLIST, "Cat�logo de veh�culos en venta", string, "Ver", "Cerrar");
	return 1;
}

Dialog:Car_Dealer_Step_1(playerid, response, listitem, inputtext[])
{
	new dealership = GetNearestDealership(playerid, 1.0),
	    aux = 0;
	
	if(dealership == -1)
     	return SendClientMessage(playerid, COLOR_YELLOW2, "Error: debes estar en una concesionaria.");

	if(response)
	{
		if(listitem == 0)
			cmd_vercatalogo(playerid, "");
  		else
  		{
  		    for(new i = 0; i < MAX_DEALERSHIP_VEHICLES; i++)
  		    {
  		    	if(DealershipVehicles[dealership][i] != 0)
  		    	    aux++;
				if(aux == listitem)
				{
                    carModelSelection[playerid] = DealershipVehicles[dealership][i];
                    break;
				}
			}
			Dialog_Show(playerid, Car_Dealer_Step_2, DIALOG_STYLE_INPUT, "Selecci�n de color primario", "Ingrese a continuaci�n el n�mero de color {00FF00}primario{a9c4e4} que desea.\n\nColores b�sicos:\n\t0 - Negro\n\t1 - Blanco\n\t2 - Azul\n\t3 - Rojo\n\t6 - Amarillo\n\t16 - Verde\n\nPara ver una lista mas amplia de colores, visite nuestro cat�logo online:\n\thttp://wiki.sa-mp.com/wiki/Vehicle_Color_IDs", "Continuar", "Volver");
			Dealership_CreatePreviewModel(playerid, carModelSelection[playerid], 3, 3);
		}
	}
 	return 1;
}

Dialog:Car_Dealer_Step_2(playerid, response, listitem, inputtext[])
{
	if(response)
	{
		new color1 = strval(inputtext);
		
		if(color1 < 0 || color1 > 255)
		{
			Dialog_Show(playerid, Car_Dealer_Step_2, DIALOG_STYLE_INPUT, "Selecci�n de color primario", "Ingrese a continuaci�n el numero de color {00FF00}primario{a9c4e4} que desea.\n\nColores b�sicos:\n\t0 - Negro\n\t1 - Blanco\n\t2 - Azul\n\t3 - Rojo\n\t6 - Amarillo\n\t16 - Verde\n\nPara ver una lista mas amplia de colores, visite nuestro cat�logo online:\n\thttp://wiki.sa-mp.com/wiki/Vehicle_Color_IDs", "Continuar", "Volver");
			return SendClientMessage(playerid, COLOR_YELLOW2, "Ingresa un n�mero de color v�lido (0-255).");
		}
		
		carColor1Selection[playerid] = color1;
        Dealership_SetPreviewModelColor(playerid, carColor1Selection[playerid], 3);
		Dialog_Show(playerid, Car_Dealer_Step_3, DIALOG_STYLE_INPUT, "Selecci�n de color secundario", "Ingrese a continuaci�n el n�mero de color {00FF00}secundario{a9c4e4} que desea.\n\nColores b�sicos:\n\t0 - Negro\n\t1 - Blanco\n\t2 - Azul\n\t3 - Rojo\n\t6 - Amarillo\n\t16 - Verde\n\nPara ver una lista mas amplia de colores, visite nuestro cat�logo online:\n\thttp://wiki.sa-mp.com/wiki/Vehicle_Color_IDs", "Continuar", "Volver");
	}
	else
 	{
	   	cmd_vercatalogo(playerid, "");
	   	Dealership_DestroyPreviewModel(playerid);
	}
 	return 1;
}

Dialog:Car_Dealer_Step_3(playerid, response, listitem, inputtext[])
{
	if(response)
	{
		new color2 = strval(inputtext),
		    dealership = GetNearestDealership(playerid, 1.0);

		if(dealership == -1)
     		return SendClientMessage(playerid, COLOR_YELLOW2, "Error: debes estar en una concesionaria.");
		if(color2 < 0 || color2 > 255)
		{
			Dialog_Show(playerid, Car_Dealer_Step_3, DIALOG_STYLE_INPUT, "Selecci�n de color secundario", "Ingrese a continuaci�n el numero de color {00FF00}secundario{a9c4e4} que desea.\n\nColores b�sicos:\n\t0 - Negro\n\t1 - Blanco\n\t2 - Azul\n\t3 - Rojo\n\t6 - Amarillo\n\t16 - Verde\n\nPara ver una lista mas amplia de colores, visite nuestro cat�logo online:\n\thttp://wiki.sa-mp.com/wiki/Vehicle_Color_IDs", "Continuar", "Volver");
			return SendClientMessage(playerid, COLOR_YELLOW2, "Ingresa un n�mero de color v�lido (0-255).");
		}

        carColor2Selection[playerid] = color2;
		Dealership_SetPreviewModelColor(playerid, carColor1Selection[playerid], carColor2Selection[playerid]);
		Dialog_Show(playerid, Car_Dealer_Step_4, DIALOG_STYLE_LIST, "Selecciona la forma de pago que usar�s", "Con efectivo\nTransferencia bancaria (recargo del 3%)", "Continuar", "Volver");
	}
	else
	{
	    Dialog_Show(playerid, Car_Dealer_Step_2, DIALOG_STYLE_INPUT, "Selecci�n de color primario", "Ingrese a continuaci�n el numero de color {00FF00}primario{a9c4e4} que desea.\n\nColores b�sicos:\n\t0 - Negro\n\t1 - Blanco\n\t2 - Azul\n\t3 - Rojo\n\t6 - Amarillo\n\t16 - Verde\n\nPara ver una lista mas amplia de colores, visite nuestro cat�logo online:\n\thttp://wiki.sa-mp.com/wiki/Vehicle_Color_IDs", "Continuar", "Volver");
	}
 	return 1;
}

Dialog:Car_Dealer_Step_4(playerid, response, listitem, inputtext[])
{
	new dealership = GetNearestDealership(playerid, 1.0),
	    str[150];

	if(dealership == -1)
     	return SendClientMessage(playerid, COLOR_YELLOW2, "Error: debes estar en una concesionaria.");

	if(response)
	{
		if(listitem == 0)
		{
			carPaymentSelection[playerid] = DEALERSHIP_PAYMENT_CASH;
			format(str, sizeof(str), "Detalles del pedido:\n\n\tModelo: %s\n\tColor primario: %d\n\tColor secundario: %d\n\tPrecio: $%d\n\tForma de pago: Pago en efectivo", GetVehicleModelName(carModelSelection[playerid]), carColor1Selection[playerid], carColor2Selection[playerid], GetVehicleModelPrice(carModelSelection[playerid], 100));
		}
  		else if(listitem == 1)
  		{
  		    carPaymentSelection[playerid] = DEALERSHIP_PAYMENT_BANK;
  		    format(str, sizeof(str), "Detalles del pedido:\n\n\tModelo: %s\n\tColor primario: %d\n\tColor secundario: %d\n\tPrecio: $%d + 3%%\n\tForma de pago: Transferencia bancaria", GetVehicleModelName(carModelSelection[playerid]), carColor1Selection[playerid], carColor2Selection[playerid], GetVehicleModelPrice(carModelSelection[playerid], 100));
		}
		Dialog_Show(playerid, Car_Dealer_Step_5, DIALOG_STYLE_MSGBOX, "Confirmaci�n de la compra", str, "Comprar", "Cancelar");
	}
	else
	{
	    Dialog_Show(playerid, Car_Dealer_Step_3, DIALOG_STYLE_INPUT, "Selecci�n de color secundario", "Ingrese a continuaci�n el numero de color {00FF00}secundario{a9c4e4} que desea.\n\nColores b�sicos:\n\t0 - Negro\n\t1 - Blanco\n\t2 - Azul\n\t3 - Rojo\n\t6 - Amarillo\n\t16 - Verde\n\nPara ver una lista mas amplia de colores, visite nuestro cat�logo online:\n\thttp://wiki.sa-mp.com/wiki/Vehicle_Color_IDs", "Continuar", "Volver");
	}
	return 1;
}


Dialog:Car_Dealer_Step_5(playerid, response, listitem, inputtext[])
{
    Dealership_DestroyPreviewModel(playerid);
    
	if(response)
	{
		new dealership = GetNearestDealership(playerid, 1.0),
		    price = GetVehicleModelPrice(carModelSelection[playerid], 100),
		    dealershipSlot = Dealership_SearchCar(dealership, carModelSelection[playerid]),
 		   	newveh = -1,
		    playerName[MAX_PLAYER_NAME],
		    str[64];

		if(dealership == -1)
     		return SendClientMessage(playerid, COLOR_YELLOW2, "Error: debes estar en una concesionaria.");
		if(dealershipSlot == -1)
		    return SendClientMessage(playerid, COLOR_YELLOW2, "Error: ese modelo ya no se encuentra a la venta.");
		if(getPlayerFreeKeySlots(playerid) < 1)
			return SendClientMessage(playerid, COLOR_YELLOW2, "Error: no tienes m�s espacio en tu llavero para otra llave.");
		if(carPaymentSelection[playerid] == DEALERSHIP_PAYMENT_CASH && GetPlayerCash(playerid) < price)
        	return SendClientMessage(playerid, COLOR_YELLOW2,"* Vendedor: �Vuelve cuando tengas el dinero en efectivo suficiente!");
        if(carPaymentSelection[playerid] == DEALERSHIP_PAYMENT_BANK && PlayerInfo[playerid][pBank] < price / 100 * 103)
        	return SendClientMessage(playerid, COLOR_YELLOW2,"* Vendedor: �No tienes suficiente dinero en tu cuenta bancaria!");

		for(new vid = 1; vid < MAX_VEH; vid++)
		{
			if(VehicleInfo[vid][VehType] == VEH_NONE)
			{
			    newveh = vid;
			    break;
			}
	    }
	    if(newveh == -1) // Si no encontr� ningun auto de tipo VEH_NONE es pq ya no hay mas lugar dentro de los MAX_VEH
			return SendClientMessage(playerid, COLOR_YELLOW2, "[OOC]: No hay m�s espacio para autos en el servidor. REPORTAR A UN ADMIN.");

		if(carPaymentSelection[playerid] == DEALERSHIP_PAYMENT_CASH)
			GivePlayerCash(playerid, -price);
		else
		    PlayerInfo[playerid][pBank] -= price / 100 * 103; // Recargo 3%
		    
	 	GetPlayerName(playerid, playerName, sizeof(playerName));
		VehicleInfo[newveh][VehType] = VEH_OWNED;
		VehicleInfo[newveh][VehOwnerSQLID] = PlayerInfo[playerid][pID];
		VehicleInfo[newveh][VehOwnerName] = playerName;
		VehicleInfo[newveh][VehColor1] = carColor1Selection[playerid];
		VehicleInfo[newveh][VehColor2] = carColor2Selection[playerid];
		VehicleInfo[newveh][VehModel] = carModelSelection[playerid];
		
		format(VehicleInfo[newveh][VehPlate], 32, "%c%c%c-%d%d%d",65+random(26),65+random(26),65+random(26),random(10),random(10),random(10));

		VehicleInfo[newveh][VehPosX] = Dealership[dealership][dParkingX];
		VehicleInfo[newveh][VehPosY] = Dealership[dealership][dParkingY];
		VehicleInfo[newveh][VehPosZ] = Dealership[dealership][dParkingZ];
		VehicleInfo[newveh][VehAngle] = Dealership[dealership][dParkingAngle];

		VehicleInfo[newveh][VehFuel] = 100;
		SendFMessage(playerid, COLOR_YELLOW2, "�Felicidades, has comprado un %s! El veh�culo se encuentra en el estacionamiento de la concesionaria.", GetVehicleModelName(carModelSelection[playerid]));
		SendClientMessage(playerid, COLOR_WHITE, "Recuerda estacionarlo fuera de la concesionaria con '/vehestacionar'. De no hacerlo podr� ser incautado.");

	 	DestroyVehicle(newveh);
		newveh = CreateVehicle(VehicleInfo[newveh][VehModel], VehicleInfo[newveh][VehPosX], VehicleInfo[newveh][VehPosY], VehicleInfo[newveh][VehPosZ], VehicleInfo[newveh][VehAngle], VehicleInfo[newveh][VehColor1], VehicleInfo[newveh][VehColor2], -1);

		addKeyToPlayer(playerid, newveh, playerid);
		SaveVehicle(newveh);
		SetVehicleNumberPlate(newveh, VehicleInfo[newveh][VehPlate]);
		SetVehicleToRespawn(newveh);
		format(str, sizeof(str), "Mod:%d C1:%d C2:%d", carModelSelection[playerid], carColor1Selection[playerid], carColor2Selection[playerid]);
		VehicleLog(newveh, playerid, INVALID_PLAYER_ID, "/vehcomprar", str);
	}
 	return 1;
}

Dealership_CreatePreviewModel(playerid, carmodel, color1, color2)
{
	// Frente
	PTD_Concesionaria[playerid][0] = CreatePlayerTextDraw(playerid, 480, 150, " ");
	PlayerTextDrawFont(playerid, PTD_Concesionaria[playerid][0], TEXT_DRAW_FONT_MODEL_PREVIEW);
	PlayerTextDrawBackgroundColor(playerid, PTD_Concesionaria[playerid][0], 0x00000090);
	PlayerTextDrawTextSize(playerid, PTD_Concesionaria[playerid][0], 150.0, 112.0);
	PlayerTextDrawSetPreviewModel(playerid, PTD_Concesionaria[playerid][0], carmodel);
	PlayerTextDrawSetPreviewRot(playerid, PTD_Concesionaria[playerid][0], -20.0, 0.0, -28.0, 0.85);
	PlayerTextDrawSetPreviewVehCol(playerid, PTD_Concesionaria[playerid][0], color1, color2);
	PlayerTextDrawShow(playerid, PTD_Concesionaria[playerid][0]);
	// Cola
	PTD_Concesionaria[playerid][1] = CreatePlayerTextDraw(playerid, 480, 262, " ");
	PlayerTextDrawFont(playerid, PTD_Concesionaria[playerid][1], TEXT_DRAW_FONT_MODEL_PREVIEW);
	PlayerTextDrawBackgroundColor(playerid, PTD_Concesionaria[playerid][1], 0x00000090);
	PlayerTextDrawTextSize(playerid, PTD_Concesionaria[playerid][1], 150.0, 112.0);
	PlayerTextDrawSetPreviewModel(playerid, PTD_Concesionaria[playerid][1], carmodel);
	PlayerTextDrawSetPreviewRot(playerid, PTD_Concesionaria[playerid][1], -20.0, 0.0, 152.0, 0.85);
	PlayerTextDrawSetPreviewVehCol(playerid, PTD_Concesionaria[playerid][1], color1, color2);
	PlayerTextDrawShow(playerid, PTD_Concesionaria[playerid][1]);
}

Dealership_SetPreviewModelColor(playerid, color1, color2)
{
	// Frente
	PlayerTextDrawSetPreviewVehCol(playerid, PTD_Concesionaria[playerid][0], color1, color2);
    PlayerTextDrawShow(playerid, PTD_Concesionaria[playerid][0]);
    // Cola
   	PlayerTextDrawSetPreviewVehCol(playerid, PTD_Concesionaria[playerid][1], color1, color2);
    PlayerTextDrawShow(playerid, PTD_Concesionaria[playerid][1]);
}

Dealership_DestroyPreviewModel(playerid)
{
	// Frente
	PlayerTextDrawDestroy(playerid, PTD_Concesionaria[playerid][0]);
	// Cola
	PlayerTextDrawDestroy(playerid, PTD_Concesionaria[playerid][1]);
}
