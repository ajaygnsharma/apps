function clearText(thefield){
	if (thefield.value==""){
		thefield.value = thefield.defaultValue
	}
}

function fillText(thefield){
	if (thefield.value == thefield.defaultValue){
		(thefield.value="")
	}
} 

if(top != self) { top.location = location }

function checksearch(fieldvar){
	if (fieldvar.value == "Enter Search Term"){
		alert("Please enter a term to search for");
		return false;
	}
	else
	{
		return true;
	}
}
		
function startList() {
if (document.all&&document.getElementById) {
	navRoot = document.getElementById("hedMenu");
		for (i=0; i<navRoot.childNodes.length; i++) {
			node = navRoot.childNodes[i];
			if (node.nodeName=="LI") {
				node.onmouseover=function() {
					this.className+=" over";
				}
				node.onmouseout=function() {
					this.className=this.className.replace(" over", "");
				}
			}
		}
	}
}
		
function focustopStory (n) {
	for (i = 0; i < numtopStories; i++) {
		if (i == n) {
			topStoryTeasers[i].style.zIndex = '1';
			topStoryIntros[i].style.zIndex = '1';
			topStoryHeadlines[i].style.background = '##e6e6e6';
			topStoryHeadlines[i].style.color = '##000';
		}
		else {
			topStoryTeasers[i].style.zIndex = '0';
			topStoryIntros[i].style.zIndex = '0';
			topStoryHeadlines[i].style.background = '##fff';
			topStoryHeadlines[i].style.color = '##666';
		}
	}
}

function rotatetopStory () {
  focustopStory(currenttopStory);
  currenttopStory = (currenttopStory + 1) % numtopStories;
  topStoryTimer = setTimeout("rotatetopStory()", 10000);
}

function mouseovertopStory (n) {
  clearTimeout(topStoryTimer);
  focustopStory(n - 1);
}

function mouseouttopStory (n) {
  currenttopStory = n - 1;
  clearTimeout(topStoryTimer);
  topStoryTimer = setTimeout("rotatetopStory()", 0)
}

function inittopStoryBox () {
  var topStoryBox = document.getElementById('current-issue');
  if(!topStoryBox) return;
  var topStoryContainer = topStoryBox.getElementsByTagName('ul');
  topStoryTeasers = topStoryBox.getElementsByTagName('img');
  topStoryIntros = topStoryBox.getElementsByTagName('p');
  topStoryHeadlines = topStoryContainer[0].getElementsByTagName('a');
  numtopStories = topStoryTeasers.length;
  currenttopStory = 0;
  rotatetopStory();
}

function winPop(loc)	{
	window.open(loc,'ultralink','width=500,height=400,resizable=yes,scrollbars=1');
}

function Images() {
  window.open('ImagesHelp.cfm','Images','toolbar=no,scrollbars=no,width=550,height=250');
}

<!--- write out the bug fix for the font issue in firefox --->
if(navigator.userAgent.indexOf("Firefox") > 0) {
	document.write("<style>p {font-size:12px;} ul,ol,li {font-size:12px;}</style>");
}	

function Details(Path, Width, Height) {
  window.open(Path,'Details','toolbar=no,scroll=yes,'+Width+Height);
}

function ParentFocus(){
	opener.location='/Info/Index.cfm?Action=ContactUs';
	opener.focus();
}

function PopUpWindow(theURL,winName,features) {
	  window.open(theURL,winName,features);
}

function eddiesub() {
  window.open('http://www.submag.com/sub/ed?pk=ede','EddieSub','toolbar=no,scroll=yes');
  document.magssub.eddiesubbox.checked=false;
}

function showTab(index) {
	var panel = document.getElementById('relatedArticlesContainer');
	var tabList = document.getElementById('blankTab');
	// set selected tab 
	var tabIndex = 1;
	for(i=0; i<tabList.childNodes.length; i++) {
		tabItem = tabList.childNodes[i];
		if(tabItem.nodeName.toLowerCase() == 'div') {
			if(tabIndex==index) tabItem.id = 'selectedTab';
			else tabItem.id = 'unselectedTab';
			tabIndex++;
		}
	}
	// show appropriate panel
	var panelIndex = 1;
	for(i=0; i<panel.childNodes.length; i++) {
		panelItem = panel.childNodes[i];
		if(panelItem.nodeName.toLowerCase() == 'div' && panelItem.id != 'blankTab') {
			if(panelIndex==index) panelItem.style.display = 'block';
			else panelItem.style.display = 'none';
			panelIndex++;
		}
	}
}