#if defined _isamp_descripcion_yo
#endinput
#endif 
#define _isamp_descripcion_yo
#define MAX_CARACTERES 80
CMD:yo(playerid, params[])
{
	new str[128];
	new text[81];
	if(sscanf(params, "s[90]", text)) return SendClientMessage (playerid, COLOR_LIGHTYELLOW2, "{5CCAF1}[Sintaxis]:{C8C8C8} /yo [Descripci�n] (Recuerda que solo se permiten 80 caracteres) ");
	{
		if(text[80] > MAX_CARACTERES)return SendClientMessage (playerid, COLOR_YELLOW2, "Has pasado el limite de caracteres, reduce tu descripci�n.");
		format (str, sizeof (str), "{C8C8C8}Tu descripci�n ahora es:{FFFFFF} %s", text);
		SendClientMessage (playerid, -1, str);
		SetPlayerChatBubble(playerid, text, COLOR_RED, 10.0 , 100000000000 );
		format(str, sizeof(str), "{C8C8C8}Para borrar tu descripci�n usa /yob");
		SendClientMessage (playerid, -1, str);
	}
	return 1;
}

CMD:yob(playerid, params[])
{
	new str[128];
	format (str, sizeof (str), "{C8C8C8}Borraste tu descripci�n, para volver a poner una usa el comando /yo");
	SendClientMessage(playerid, -1, str);
	SetPlayerChatBubble(playerid,"", COLOR_RED, 10.0, 1000);
	return 1;
}