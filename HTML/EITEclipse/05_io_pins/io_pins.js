	var s_no = [ "1","2","3","4"]; 

    var io_name = ["io1","io2","io3","io4","io5","io6","io7","io8"];
    var i_id = ["i1","i2","i3","i4","i5","i6","i7","i8"]; 
	var o_id = ["o1","o2","o3","o4","o5","o6","o7","o8"];
	var fio_status = [ "","","",""];
	var i_chk = [ "checked","checked","checked","checked"];
	var o_chk = [ " "," "," "," "];
    	
	var p_name = ["p1c", "p2c", "p3c", "p4c", "p5c", "p6c", "p7c", "p8c"];
	var h_id = ["p1ch","p2ch","p3ch","p4ch","p5ch","p6ch","p7ch","p8ch"];
	var l_id = ["p1cl","p2cl","p3cl","p4cl","p5cl","p6cl","p7c1","p8c1"];
	var h_chk = [ "checked","checked","checked","checked"];
	var l_chk = [ " "," "," "," "];
    
	var image_source = [ "grey.jpg","grey.jpg","grey.jpg","grey.jpg"];
	
	var s_ino = ["1", "2", "3", "4"]; 
    var s_ono = ["4", "5", "6", "7"];
    var ci_name = ["c_i1","c_i2","c_i3", "c_i4"];
    var ci_i_id = ["c_ii1","c_ii2","c_ii3", "c_ii4"]; 
	var ci_o_id = ["c_io1","c_io2","c_io3", "c_io4"];
	var cio_status = [ "disabled","disabled","disabled","disabled"];
	var ci_i_chk = [ "checked","checked","checked","checked"];
	var ci_o_chk = [ "","","",""];
    
	var ct_name = ["c_t1","c_t2","c_t3","c_t4"];
	var ct_i_id = ["c_ti1","c_ti2","c_ti3","c_ti4"];
	var ct_o_id = ["c_t01","c_t02","c_t03","c_t04"];
	var ct_i_chk = [ "checked","checked","checked","checked"];
	var ct_o_chk = [ "","","",""];
    
    document.writeln("<tr> <td> <table border=\"1\" align=\"center\"> <tr> <th> </th> <th>Direction</th> <th>Output Condition</th> <th>Input Condition</th></tr>");
	  for (var i = 0; i < s_no.length; i++) 
	  {
		document.writeln("<tr> <td>" + s_no[i] + "</td>\n");
		
		/* Input Radio Button */
		document.writeln("<td> <input type=\"radio\" value=\"1\" name=\"" + io_name[i] + "\"");
        document.writeln("id=\"" + i_id[i]   + "\"");
		document.writeln(i_chk[i]    + "=\"" + i_chk[i]    + "\"");
		document.writeln(fio_status[i] + "=\"" + fio_status[i] + "\"");
		document.writeln(">Input ");
		
		/* Output Radio Button*/
		document.writeln("<input type=\"radio\" value=\"2\" name=\"" + io_name[i] + "\"");
        document.writeln("id=\"" + o_id[i]   + "\"");
		document.writeln(o_chk[i]    + "=\"" + o_chk[i]    + "\"");
		document.writeln(fio_status[i] + "=\"" + fio_status[i] + "\"");
		document.writeln(">Output </td>");
		
		/* High Radio Button*/
		document.writeln("<td><input type=\"radio\" value=\"1\" name=\"" + p_name[i] + "\"");
        document.writeln("id=\"" + h_id[i]   + "\"");
		document.writeln(h_chk[i]    + "=\"" + h_chk[i]    + "\"");
		document.writeln(fio_status[i] + "=\"" + fio_status[i] + "\"");
		document.writeln(">High");
		
		/* Low Radio Button*/
		document.writeln("<input type=\"radio\" value=\"2\" name=\"" + p_name[i] + "\"");
        document.writeln("id=\"" + l_id[i]   + "\"");
		document.writeln(l_chk[i]    + "=\"" + l_chk[i]    + "\"");
		document.writeln(fio_status[i] + "=\"" + fio_status[i] + "\"");
		document.writeln(">Low</td>");
		
		document.writeln("<td> <img src=\"" + image_source[i] + "\"> </td>");
		
		document.writeln("</tr>\n");
	  }
    document.writeln("</table> <img src=\"green.jpg\"> High <br> <img src=\"grey.jpg\"> Low </td> </tr>");
	
	document.writeln("<input type=\"checkbox\" name=\"cio\" id=\"cio\"> Conditional I/O Pins");

    if(s_no.length > 1)
	{
	  document.writeln("<tr> <td> <h2> CONDITIONAL I/O </h2> </td> </tr> <tr> <td> <table width=\"100%\" align=\"center\" cellspacing=\"0\" cellpadding=\"0\" border=\"1\"><tr> <th> Input Pin </th> <th> IF </th> <th> Output Pin </th> <th> Then </th> </tr>");
	  for (var j = 0; j < ci_i_chk.length; j++) 
	  {
		document.writeln("<tr> <td>" + s_ino[j] + "</td>\n");
				
		/* Input Radio Button */
		document.writeln("<td> <input type=\"radio\" value=\"1\" name=\"" + ci_name[j] + "\"");
        document.writeln("id=\"" + ci_i_id[j]   + "\"");
		document.writeln(ci_i_chk[j]    + "=\"" + ci_i_chk[j]    + "\"");
		document.writeln(cio_status[j] + "=\"" + cio_status[j] + "\"");
		document.writeln(">High ");
		
		/* Output Radio Button*/
		document.writeln("<input type=\"radio\" value=\"2\" name=\"" + ci_name[j] + "\"");
        document.writeln("id=\"" + ci_o_id[j]   + "\"");
		document.writeln(ci_o_chk[j]    + "=\"" + ci_o_chk[j]    + "\"");
		document.writeln(cio_status[j] + "=\"" + cio_status[j] + "\"");
		document.writeln(">Low </td>");
		
		/* High Radio Button */
		document.writeln("<td>" + s_ono[j] + "</td>\n");
		
		document.writeln("<td> <input type=\"radio\" value=\"1\" name=\"" + ct_name[j] + "\"");
        document.writeln("id=\"" + ct_i_id[j]   + "\"");
		document.writeln(ct_i_chk[j]    + "=\"" + ct_i_chk[j]    + "\"");
		document.writeln(cio_status[j] + "=\"" + cio_status[j] + "\"");
		document.writeln(">High ");
		
		/* Output Radio Button*/
		document.writeln("<input type=\"radio\" value=\"2\" name=\"" + ct_name[j] + "\"");
        document.writeln("id=\"" + ct_o_id[j]   + "\"");
		document.writeln(ct_o_chk[j]    + "=\"" + ct_o_chk[j]    + "\"");
		document.writeln(cio_status[j] + "=\"" + cio_status[j] + "\"");
		document.writeln(">Low </td>");
		
		document.writeln("</tr>\n");
	  }
	  document.writeln("</table> </td> </tr>");
    }
	else
	{
	  document.writeln(" ");
	}
	