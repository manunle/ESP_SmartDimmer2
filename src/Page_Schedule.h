char *ROnTimeH = "ROnTimeH";
char *ROffTimeH = "ROffTimeH";
char *ROnTimeM = "ROnTimeM";
char *ROffTimeM = "ROffTimeM";

bool validSchedule(Sched shed)
{
    int tot = 0;
    for(int i=0;i<7;i++)
    {
        tot = tot + shed.wdays[i];
    }
    return tot > 0;
}

void send_schedule_html()
{
    char cidx[10];
    char cjdx[10];
    char crdx[10];
    if(server.args() == 0)
    {
        char *pagen = (char*)malloc(17384);
        char *line = (char*)malloc(256);
        pagen[0] = 0;
        line[0] = 0;
        strcpy(line,"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\n");
        strcpy(pagen,line);
        pagen[strlen(line)]=0;
        strcat(pagen,"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n");
        strcat(pagen,"<a href=\"/\"  class=\"btn btn--s\"><</a>&nbsp;&nbsp;<strong>Schedule Settings</strong>\n");
        strcat(pagen,"<hr>\n");
        strcat(pagen,"<form action=\"\" method=\"post\">");
        strcat(pagen,"On at Sunset 1 <input type=\"checkbox\" id=\"OnAtSunset1\" name=\"OnAtSunset1\">");
        strcat(pagen," Off at Sunrise 1 <input type=\"checkbox\" id=\"OffAtSunrise1\" name=\"OffAtSunrise1\">");
        strcat(pagen,"<br>");
        strcat(pagen,"On at Sunset 2 <input type=\"checkbox\" id=\"OnAtSunset2\" name=\"OnAtSunset2\">");
        strcat(pagen," Off at Sunrise 2 <input type=\"checkbox\" id=\"OffAtSunrise2\" name=\"OffAtSunrise2\">");
        strcat(pagen,"<br>");
        strcat(pagen,"<table border=\"0\"  cellspacing=\"0\" cellpadding=\"3\" >\n");
        strcat(pagen,"<col style=\"width:8%\"><col style=\"width:8%\"><col style=\"width:2%\"><col style=\"width:2%\"><col style=\"width:2%\"><col style=\"width:2%\"><col style=\"width:2%\"><col style=\"width:2%\"><col style=\"width:2%\">\n");
        strcat(pagen,"<tr><td align=\"left\">ON</td><td align=\"left\">OFF</td><td>Sun</td><td>Mon</td><td>Tue</td><td>Wed</td><td>Thu</td><td>Fri</td><td>Sat</td><td></td></tr>\n");
        strcat(pagen,"<tr><td>Relay ");
        strcat(pagen,"</td></tr>\n");
        for(int i=0;i<10;i++)
        {
            itoa(i,cidx,10);
            strcpy(line,cidx);
            strcat(pagen,"<td><input type=\"text\" id=\""); strcat(pagen,ROnTimeH);
            strcat(pagen,line);
            strcat(pagen,"\" name=\""); strcat(pagen,ROnTimeH);
            strcat(pagen,line);
            strcat(pagen,"\" value=\"\" size=2>:");
            strcat(pagen,"<input type=\"text\" id=\""); strcat(pagen,ROnTimeM);
            strcat(pagen,line);
            strcat(pagen,"\" name=\""); strcat(pagen,ROnTimeM);
            strcat(pagen,line);
            strcat(pagen,"\" value=\"\" size=2></td>\n");
            strcat(pagen,"<td><input type=\"text\" id=\""); strcat(pagen,ROffTimeH);
            strcat(pagen,line);
            strcat(pagen,"\" name=\""); strcat(pagen,ROffTimeH);
            strcat(pagen,line);
            strcat(pagen,"\" value=\"\" size=2>:");
            strcat(pagen,"<input type=\"text\" id=\""); strcat(pagen,ROffTimeM);
            strcat(pagen,line);
            strcat(pagen,"\" name=\""); strcat(pagen,ROffTimeM);
            strcat(pagen,line);
            strcat(pagen,"\" value=\"\" size=2></td>\n");
            for(int j=0;j<7;j++)
            {
                itoa(j,cjdx,10);
                strcpy(line,cidx);
                strcat(line,cjdx);
                strcat(pagen,"<td><input type=\"checkbox\" id=\"RWD"); strcat(pagen,line); strcat(pagen,"\" name=\"RWD"); strcat(pagen,line); strcat(pagen,"\"></td>");
            }
            strcat(pagen,"</tr>\n");
        }
        strcat(pagen,"<tr><td colspan=\"2\" align=\"center\"><input type=\"submit\" style=\"width:150px\" class=\"btn btn--m btn--blue\" value=\"Save\"></td></tr>");
        strcat(pagen,"</table></form><script>\n");
        strcat(pagen,"window.onload = function ()\n{\n	load(\"style.css\",\"css\", function() \n	{\n		load(\"microajax.js\",\"js\", function() \n		{\n				setValues(\"/admin/schedulevalues\");\n		});\n	});\n}\n");
        strcat(pagen,"function load(e,t,n){\n  if(\"js\"==t){\n    var a=document.createElement(\"script\");\n    a.src=e,a.type=\"text/javascript\",a.async=!1,a.onload=function(){\n      n()\n    },document.getElementsByTagName(\"head\")[0].appendChild(a)\n  }else if(\"css\"==t){\n    var a=document.createElement(\"link\");\n    a.href=e,a.rel=\"stylesheet\",a.type=\"text/css\",a.async=!1,a.onload=function(){\n      n()\n    },document.getElementsByTagName(\"head\")[0].appendChild(a)\n  }\n}");
        strcat(pagen,"</script>");
        Serial.println(String(strlen(pagen)));
        server.send ( 200, "text/html", pagen );
        free(pagen);
        free(line);
    }
    else
    {
        for(int i=0;i<10;i++)
            for(int j=0;j<7;j++)
                config.RSchedule[i].wdays[j] = false;
        config.RSchedule[0].onatsunset = false;
        config.RSchedule[1].onatsunset = false;
        config.RSchedule[0].offatsunrise = false;
        config.RSchedule[1].offatsunrise = false;
        config.RSchedule[0].onatsunset = false;
        config.RSchedule[1].onatsunset = false;
        config.RSchedule[0].offatsunrise = false;
        config.RSchedule[1].offatsunrise = false;
   		for ( uint8_t a = 0; a < server.args(); a++ ) 
        {
			Serial.print(server.argName(a));
            if(server.argName(a) == "OnAtSunset1")
            {                        
                Serial.println("turning on 00 onatsunset");
                config.RSchedule[0].onatsunset = true;
            }
            if(server.argName(a) == "OnAtSunset2")
                config.RSchedule[1].onatsunset = true;
            if(server.argName(a) == "OffAtSunrise1")
                config.RSchedule[0].offatsunrise = true;
            if(server.argName(a) == "OffAtSunrise2")
                config.RSchedule[1].offatsunrise = true;
            if(server.argName(a) == "OnAtSunset1")
                config.RSchedule[0].onatsunset = true;
            if(server.argName(a) == "OnAtSunset2")
                config.RSchedule[1].onatsunset = true;
            if(server.argName(a) == "OffAtSunrise1")
                config.RSchedule[0].offatsunrise = true;
            if(server.argName(a) == "OffAtSunrise2")
                config.RSchedule[1].offatsunrise = true;
            bool found = false;
            for(int i=0;i<10;i++)
            {
                itoa(i,cidx,10);
                char pname[20];
                strcpy(pname,"ROnTimeH");
                strcat(pname,cidx);
                if(server.argName(a) == pname)
                {
                    config.RSchedule[i].onHour=server.arg(a).toInt();
                    Serial.println(" found");
                    found = true;
                    break;
                }
                strcpy(pname,"ROffTimeH");
                strcat(pname,cidx);
                if(server.argName(a) == pname)
                {
                    config.RSchedule[i].offHour=server.arg(a).toInt();
                    Serial.println(" found");
                    found = true;
                    break;
                }
                strcpy(pname,"ROnTimeM");
                strcat(pname,cidx);
                if(server.argName(a) == pname)
                {
                    config.RSchedule[i].onMin=server.arg(a).toInt();
                    Serial.println(" found");
                    found = true;
                    break;
                }
                strcpy(pname,"ROffTimeM");
                strcat(pname,cidx);
                if(server.argName(a) == pname)
                {
                    config.RSchedule[i].offMin=server.arg(a).toInt();
                    Serial.println(" found");
                    found = true;
                    break;
                }
                for(int j=0;j<7;j++)
                {
                    itoa(j,cjdx,10);
                    strcpy(pname,"RWD");
                    strcat(pname,cidx);
                    strcat(pname,cjdx);
                    if(server.argName(a) == pname)
                    {
                        config.RSchedule[i].wdays[j] = true;
                        Serial.println(" found");
                        found = true;
                        break;
                    }
                }
            } 
            if(!found)
                Serial.println(" not");
        }
		WriteConfig();
        server.send ( 200, "text/html", "bye" );
    }
}

void send_schedule_configuration_values_html()
{
    char cidx[10];
    char cjdx[10];
    char line[512];
    char *cvalues = (char*)malloc(8192);
    char *tmp = (char*)malloc(512);
    char *checked = "checked";
    cvalues[0] = 0;
    strcat(cvalues,"OnAtSunset1|"); if(config.RSchedule[0].onatsunset) strcat(cvalues,checked); strcat(cvalues,"|chk\n");
    strcat(cvalues,"OnAtSunset2|"); if(config.RSchedule[1].onatsunset) strcat(cvalues,checked); strcat(cvalues,"|chk\n");
    strcat(cvalues,"OffAtSunrise1|"); if(config.RSchedule[0].offatsunrise) strcat(cvalues,checked); strcat(cvalues,"|chk\n");
    strcat(cvalues,"OffAtSunrise2|"); if(config.RSchedule[1].offatsunrise) strcat(cvalues,checked); strcat(cvalues,"|chk\n");
    for(int i=0;i<10;i++)
    {
        itoa(i,cidx,10);
        strcpy(line,cidx);
        itoa(config.RSchedule[i].offHour,tmp,10);
        strcat(cvalues,ROffTimeH);
        strcat(cvalues,line);
        strcat(cvalues,"|");
        strcat(cvalues,tmp);
        strcat(cvalues,"|input\n");
        itoa(config.RSchedule[i].onHour,tmp,10);
        strcat(cvalues,ROnTimeH);
        strcat(cvalues,line);
        strcat(cvalues,"|");
        strcat(cvalues,tmp);
        strcat(cvalues,"|input\n");
        itoa(config.RSchedule[i].offMin,tmp,10);
        strcat(cvalues,ROffTimeM);
        strcat(cvalues,line);
        strcat(cvalues,"|");
        strcat(cvalues,tmp);
        strcat(cvalues,"|input\n");
        itoa(config.RSchedule[i].onMin,tmp,10);
        strcat(cvalues,ROnTimeM);
        strcat(cvalues,line);
        strcat(cvalues,"|");
        strcat(cvalues,tmp);
        strcat(cvalues,"|input\n");
        for(int j=0;j<7;j++)
        {
            itoa(j,cjdx,10);
            strcpy(line,cidx);
            strcat(line,cjdx);
            strcat(cvalues,"RWD");
            strcat(cvalues,line);
            strcat(cvalues,"|"); if(config.RSchedule[i].wdays[j]) strcat(cvalues,checked); strcat(cvalues,"|chk\n");
        }
    }
	server.send ( 200, "text/plain", cvalues);
    Serial.println(cvalues);
    Serial.println(String(strlen(cvalues)));
	Serial.println(__FUNCTION__); 
    AdminTimeOutCounter=0;
    free(cvalues);
    free(tmp);
//  Serial.println(values);
}
