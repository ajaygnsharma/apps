
// Popup Functions
function popwin() {		
	var popW = 400, popH = 400;
	var w = screen.availWidth;
	var h = screen.availHeight;
	var popName = "myWin";
	
	page = popwin.arguments[0];
	
	if(popwin.arguments[1]) popW = popwin.arguments[1];
	if(popwin.arguments[2]) popH = popwin.arguments[2];
	if(popwin.arguments[3]) popName = popwin.arguments[3];
	
	var leftPos = (w-popW)/2, topPos = (h-popH)/2;
	
	myWin = window.open(page,popName,"resizable=1,scrollbars=1,width="+popW+",height="+popH+",top="+topPos+",left="+leftPos)
	myWin.focus();
}

function MM_openBrWindow(theURL,winName,features) {
	window.open(theURL,winName,features);
}

function searchSelect(txt, el) {
	var field = eval("document.myform." + el);
	
	txt = txt.toLowerCase();
	txtLen = txt.length;
	
	if(txt.length == 0) {
		field.options[0].selected = true;
		return;
	}
	
	for(i=0; i < field.options.length; i++) {
		if(txt.substring(0,txtLen+1) == field.options[i].text.substring(0,txtLen).toLowerCase()) {
			field.options[i].selected = true;
			break;
		}
	}
}


//=========================================================
// FUNC:	getSelectMultIndex
// PARM:	input - ref to form select-multiple input
//  RES:	array containing selected indexes
//=========================================================
function getSelectMultIndex(input) {
	var aResult = new Array();

	for (var i=0; i < input.options.length; i++) {
		if(input.options[i].value == "-1") {input.options[i].selected = false}
		if (input.options[i].selected) aResult[aResult.length] = i;
	}	
	return aResult;
}

//=========================================================
// FUNC:	moveOption() - Moves clients from one select to the other
// PARM:	Direction (right or left)//  
//=========================================================
function moveOption(sBoxFrom, sBoxTo) {
	var tempVals
	var count
	var counter = 0
	var fields = document.myform
	var fromFieldCount = eval("fields." + sBoxFrom + ".length")
	
	tempVals = getSelectMultIndex(eval("fields." + sBoxFrom))
	count = tempVals.length
	
	for (i=0; i<count; i++) {
			//add item to the other side
			fieldCount = eval("fields." + sBoxTo + ".length")
			myVal = eval("fields." + sBoxFrom + ".options[" + (tempVals[i]-i) + "].value")
			myText = eval("fields." + sBoxFrom + ".options[" + (tempVals[i]-i) + "].text")
			eval("fields." + sBoxTo + ".options[fieldCount] = new Option(myText, myVal, false, false)")
			
			//delete the item
			eval("fields." + sBoxFrom + ".options[" + (tempVals[i]-i) + "] = null")
	}
}


function shiftOption(input, dir) {
	var field = eval("document.myform." + input);
	var tempVals = getSelectMultIndex(field);
	var aValues = new Array();

	for(i=0; i < field.options.length; i++) {
		//move the item
		if(field.options[i].selected && field.options[i].value != "-1") { 
		
			//is it out of bounds
			if(dir == 1 && i == field.options.length - 1) return;
			if(dir == -1 && i == 0) return;
			
			myVal = field.options[i].value.substring(0, field.options[i].value.indexOf("_"));
			//mySort = field.options[i].value.substring(field.options[i].value.indexOf("_") + 1);
			myText = field.options[i].text;
			
			myVal2 = field.options[i + dir].value.substring(0, field.options[i + dir].value.indexOf("_"));
			//mySort2 = field.options[i + dir].value.substring(field.options[i + dir].value.indexOf("_") + 1);
			myText2 = field.options[i + dir].text;
			
			myVal = myVal //+ "_" + mySort2;
			myVal2 = myVal2 //+ "_" + mySort;
			
			field.options[i] = new Option(myText2, myVal2);
			field.options[i + dir] = new Option(myText, myVal);				
			
			//add it to the array
			aValues[aValues.length] = field.options[i + dir].value;	
		}
	}		
	//go through and re-select them
	for(i=0; i < aValues.length; i++) {
		for(j=0; j < field.options.length; j++) {
			if(field.options[j].value == aValues[i]) field.options[j].selected = true;
		}
	}
}
	

function selectAll(input) {
	var field = eval("document.myform." + input);
		
	for(i=0; i < field.options.length; i++) {
		if(field.options[i].value == "-1") {field.options[i].selected = false;}
		else field.options[i].selected = true;
	}		
}

function limitTextarea(obj, maxlength, statusid) {
		if(obj.value.length > maxlength) obj.value = obj.value.substring(0, maxlength);
		document.getElementById(statusid).innerHTML = maxlength - obj.value.length;
}


//=========================================================
//	Dreamwaver snippet: helper for MM_validateForm()
//=========================================================

function MM_findObj(n, d) { //v4.01
  var p,i,x;  if(!d) d=document; if((p=n.indexOf("?"))>0&&parent.frames.length) {
    d=parent.frames[n.substring(p+1)].document; n=n.substring(0,p);}
  if(!(x=d[n])&&d.all) x=d.all[n]; for (i=0;!x&&i<d.forms.length;i++) x=d.forms[i][n];
  for(i=0;!x&&d.layers&&i<d.layers.length;i++) x=MM_findObj(n,d.layers[i].document);
  if(!x && d.getElementById) x=d.getElementById(n); return x;
}

//=========================================================
//	Modified Dreamwaver snippet: validates form input,
//		modified to reset invalid fields to default values,
//		and print a more user-friendly error msg
//=========================================================
function MM_validateForm() { //v4.0
  //array to hold the default field values
  var defaultvals=new Array(" First Name"," Last Name"," Email"," Street Address"," City","",""," Zip Code");
  var i,p,q,nm,test,num,min,max,errors='',args=MM_validateForm.arguments;
  for (i=0; i<(args.length-2); i+=3) { test=args[i+2]; val=MM_findObj(args[i]);
	//modified to check if the submitted value is the default value
	if (val) { nm=val.name; if ((val=val.value)!="" && val!=defaultvals[i/3]) {
      if (test.indexOf('isEmail')!=-1) { p=val.indexOf('@');
        if (p<1 || p==(val.length-1)) {
		  //the field failed validation -> reset
		  document.getElementById(nm).value=document.getElementById(nm).defaultValue; 
		  errors+='- '+nm+' must contain an e-mail address.\n';
		}
      } else if (test!='R') { num = parseFloat(val);
        if (isNaN(val)) {
		  //the field failed validation -> reset
		  document.getElementById(nm).value=document.getElementById(nm).defaultValue; 
		  errors+='- '+nm+' must contain a number.\n';
		}
        if (test.indexOf('inRange') != -1) { p=test.indexOf(':');
          min=test.substring(8,p); max=test.substring(p+1);
          if (num<min || max<num) {
		    //the field failed validation -> reset
		    document.getElementById(nm).value=document.getElementById(nm).defaultValue; 
		    errors+='- '+nm+' must contain a number between '+min+' and '+max+'.\n';
		  }
	//modified to substitute address1, ProvinceID, CountryID with something more readable
	} } } else if (test.charAt(0) == 'R')  {
		//next 3 lines were added
		if (nm=="address1") nm="streetaddress";
		if (nm=="ProvinceID") nm="state/province";
		if (nm=="CountryID") nm="country";
		errors += '- '+nm+' is required.\n'; }
	}
  } if (errors) alert('The following error(s) occurred:\n'+errors);
  document.MM_returnValue = (errors == '');
}

//=========================================================
// FUNC:	activateMenu, displays the navigation menu
//			sub-items when their parent is moused over
//=========================================================
activateMenu = function() {
/* currentStyle restricts the Javascript to IE only */
	if (document.all && document.getElementById('navMenu').currentStyle) {  
		var navroot = document.getElementById('navMenu');
		/* Get all the list items within the menu */
		var lis=navroot.getElementsByTagName("LI");  
		for (i=0; i<lis.length; i++) {
		   /* If the LI has another menu level */
			if(lis[i].lastChild.tagName=="UL"){
				/* assign the function to the LI */
				lis[i].onmouseover=function() {	
				   /* display the inner menu */
				   this.lastChild.style.display="block";
				}
				lis[i].onmouseout=function() {                       
				   this.lastChild.style.display="none";
				}
			}
		}
	}
}

//=========================================================
// DEVELOPMENT:	generateLiOvers, generateSpanOvers, generateOvers
//				more generic versions of the above method		
//=========================================================
function generateSpanOvers(rootEl) {
	if (navigator.appName == 'Microsoft Internet Explorer') {
		var spans = rootEl.getElementsByTagName("SPAN");
		for (i = 0; i < spans.length; i++) {
			spans[i].onmouseover = function() {	
				this.className += " over";
			}
			spans[i].onmouseout=function() {                       
				this.className = this.className.replace(" over", "");
			}
		}
	}
}
function generateLiOvers(rootEl) {
	if (navigator.appName == 'Microsoft Internet Explorer') {
		var lis = rootEl.getElementsByTagName("LI");
		for (i = 0; i < lis.length; i++) {
			lis[i].onmouseover = function() {	
				this.className += " over";
			}
			lis[i].onmouseout=function() {                       
				this.className = this.className.replace(" over", "");
			}
		}
	}
}
function generateOvers(rootEl) {
	generateLiOvers(rootEl);
	generateSpanOvers(rootEl);
}