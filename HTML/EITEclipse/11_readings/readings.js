var dev_na= new Array();
var dev_nb= new Array();
var dev_nc= new Array();
var dev_nd= new Array();
var i;

    document.writeln("<table border='1' cellpadding='3' cellspacing='3'>");
	document.writeln("<tbody><tr><td align='center'><table border='0'>");
    document.writeln("<tr>");
    if(dev_nc[0] == "")
	  document.writeln("<td align='center'><font size='12'color='#7FFF00'face='arial'/>" + "&nbsp;" + "</td>");
	else
	  document.writeln("<td align='center'><font size='12'color='#7FFF00'face='arial'/>" + dev_nc[0] + "</td>");
	
	if(dev_nc[0] == "")
	  document.writeln("<td align='center'><font size='12'color='#7FFF00'face='arial'/>" + "&nbsp;" + "</td>");
	else  
	  document.writeln("<td align='center'><font size='12'color='#7FFF00'face='arial'/>" + dev_nd[0] + "</td>");
	document.writeln("</tr>");
	document.writeln("</table></td></tr></tbody></table>");

	    
  for (i = 0; i < 0 ; i++)
  {
    document.writeln("<tr>");
    document.writeln("<td>" + dev_na[i] + "</td>");
    document.writeln("<td>" + dev_nb[i] + "</td>");
    document.writeln("<td>" + dev_nc[i] + "</td>");
    document.writeln("<td>" + dev_nd[i] + "</td>");
    document.writeln("</tr>");
  }
      
  function AutoReload() 
    {
      window.location.href = window.location.href;
      setTimeout("AutoReload()", 000000);
      return true;
    }
    
 