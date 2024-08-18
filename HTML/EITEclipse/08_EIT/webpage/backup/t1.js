var s_no = ["1", "2", "3", "4", "5", "6"]; 

var io_name = ["io1","io2","io3","io4","io5","io6"];
var i_id = ["i1","i2","i3","i4","i5","i6"]; 
var o_id = ["o1","o2","o3","o4","o5","o6"];
var i_status = [" "," "," "," "," "," "];
var o_status = [" "," "," "," "," "," "];
var i_chk = [<!--#exec cgi="i_chk"-->, " ", " ", " ", " ", " "];
var o_chk = [" ", " ", " ", " ", " ", " "];

var p_name = ["p1c", "p2c", "p3c", "p4c", "p5c", "p6c"];
var h_id = ["p1ch","p2ch","p3ch","p4ch","p5ch","p6ch"];
var l_id = ["p1cl","p2cl","p3cl","p4cl","p5cl","p6cl"];
var h_status = [" "," "," "," "," "," "];
var l_status = [" "," "," "," "," "," "];
var h_chk = [" ", " ", " ", " ", " ", " "];
var l_chk = [" ", " ", " ", " ", " ", " "];

function showData() 
{
  var tableData = "";
  for (var i = 0; i < io_name.length; i++) 
  {
    tableData += "<td>" + s_no[i] + "</td>\n"
    tableData += "<td> <input type=\"radio\" value=\"1\" name=\"" + io_name[i] + "\" id=\"" + i_id[i] 
    tableData += "\" \"" + i_status[i] + "" + i_chk[i] + ">Input "
    tableData += "<input type=\"radio\" value=\"2\" name=\"" + io_name[i] + "\" id=\"" + o_id[i]
    tableData += "\" \"" + o_status[i] + "\"" + o_chk[i] + "> Output </td>\n"
    tableData += "<td> <input type=\"radio\" value=\"1\" name=\"" + p_name[i] + "\" id=\"" + h_id[i]
    tableData += "\" \"" + h_status[i] + "\"" + h_chk[i] + "> High "
    tableData += "<input type=\"radio\" value=\"2\" name=\"" + p_name[i] + "\" id=\"" + l_id[i]
    tableData += "\" \"" + l_status[i] + "\"" + l_chk[i] + "> Low </td>\n"
    tableData += "</tr>\n"
  }
  document.getElementById("display").innerHTML = tableData
}
