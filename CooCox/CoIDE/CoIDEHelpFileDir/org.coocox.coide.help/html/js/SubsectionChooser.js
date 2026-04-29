 
var request; // request object
var pageUrl = "/script/Content/Ajax/GetSubsectionsList.aspx"; // the Url of the page, which takes care of Ajax calls
var sectionIdKey    = "secId";
var sleepTime       = 500; //wait a little for the subsection list to come back

//Method to create XmlHttp object based on browser
function CreateXmlHttp()
{
	if(window.XMLHttpRequest) 
	{
		try { request = new XMLHttpRequest(); }
		catch(e) {request = false; }
	}
	else if(window.ActiveXObject)
	{
		try { request = new ActiveXObject("Msxml2.XMLHTTP"); } 
		catch(e)
		{
			try { request = new ActiveXObject("Microsoft.XMLHTTP"); } 
			catch(e) { request = false; }
		}
	}
	return request;
}

function PopulateSubsections(sectionList)
{
	if(!sectionList) return;
	var selectedSectionId = sectionList.options[sectionList.selectedIndex].value;
	if (selectedSectionId > 0) {
		request = CreateXmlHttp();
		if (request) {
			request.onreadystatechange = ProcessSectionResponse;
			var queryString = "?" + sectionIdKey + "=" + selectedSectionId.toString();
			request.open("GET", pageUrl + queryString, true);
			request.send(null);
		}

		return false;
	}
	else {
		var subsectionId = document.getElementById("SubsectionClientId");
		if (!subsectionId) return
		var subsectionsList = document.getElementById(subsectionId.value);
		if (!subsectionsList) return;

		subsectionsList.length = 1;
		subsectionsList[0] = new Option("--- No Subsections ---", 0);
	}
}

// Method to be called on Asynchronous call back 
//when new question is added
function ProcessSectionResponse()
{
	if(request.readyState == 4)
	{	
		if(request.status == 200)
		{
			setTimeout('ClearAndSetSubsectionList(request.responseXML)', sleepTime);
		}
	}
}

//Clears the contents of subsectionsList and adds the subsections of currently selected section
function ClearAndSetSubsectionList(subSectionsXml)
{
	if(!subSectionsXml) return;
	
	var subsectionId = document.getElementById("SubsectionClientId");
	if(!subsectionId) return
	var subsectionsList = document.getElementById(subsectionId.value);
	if (!subsectionsList) return;
	
	//Clears the state combo box contents.
	for (var count = subsectionsList.options.length-1; count >-1; count--)
	{
		subsectionsList.options[count] = null;
	}

	var subsectionsNodes = subSectionsXml.getElementsByTagName('Subsection');
	var optionItem;
		
	//Add new states list to the state combo box.
	for (var count = 0; count < subsectionsNodes.length; count++)
	{		
		var currentNode = subsectionsNodes[count];
		var optionText	= currentNode.getAttribute('name');
		var optionValue = currentNode.getAttribute('id');
		if((optionText!= null) && (optionValue != null))
		{
			optionItem = new Option(optionText, optionValue,  false, (optionText == 'General'));
			subsectionsList.options[count] = optionItem;
		}
	}
 }