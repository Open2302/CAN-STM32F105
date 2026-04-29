// TODO: Convert to object. This is a mess.
var AC_ClientId;
var ParentTableId = 'AttTable_';
var WatermarkTxt = 'Filter (e.g. C++)';

function MakeSelect(attrId, name) {
	if (document.getElementById('UpdateRmdr')) document.getElementById('UpdateRmdr').style.display='';
}

// Shows/Hides versions checkboxes for a given attribute
function showHideVersion(element, scrollBoxId) {
	var parentNode = document.getElementById(scrollBoxId);
	if (parentNode)	{
		var versions = parentNode.getElementsByTagName("div");
		for (var i=0; i<versions.length; i++) {
			var elm=versions[i];
			if (elm && elm.attributes["parent"] && elm.attributes["parent"].value==element) {
				if (elm.style.display=="none") 
					elm.style.display="block";
				else 
					elm.style.display="none";
			}
		}
	}
	return false;
}

function ClearScrollChecks() {
	SetAttrBarChecks('sub-attr', null);
	SetAttrBarChecks('attr', null);
	var ft = document.getElementById(AC_ClientId + "_FreeText");
	if (ft) {
		ft.value = '';
		UpdateWatermark(ft, WatermarkTxt);
	}
}

function setFreeTextFromList() {
	var id = ParentTableId + AC_ClientId;
	if (id) {
		// 1. Get divs of class "attr". Get input and label. If input checked, add label to list
		// 2. Get divs of class "sub-attr". Get input and label. If input checked, add label to list
	}
}

function setListFromFreeText() {
	var id = ParentTableId + AC_ClientId;
	if (id) iterateCheckboxes(id, setCheckFromFreeText);
}

// callback for setting checkboxes based on the free text box
function setCheckFromFreeText(checkBox, name) {
	name = name.replace(/<\S[^>]*>/g, "");
	var txt = document.getElementById(AC_ClientId + "_FreeText").value.replace(/[\s]+/g, " ");
	// TODO: Scroll newly checked box into view
	checkBox.checked = (txt && name && (', ' + txt.toUpperCase() + ',').indexOf(', ' + name.toUpperCase() + ',') >= 0);
}


var idList = '';
function getSelectedScrollListAttrIds() {
	var id = ParentTableId + AC_ClientId;
	idList = '';
	if (id) iterateCheckboxes(id, addToIdList);
	return idList;
}

// callback for adding id's to the ID list
function addToIdList(checkBox, name)
{
	if (checkBox.checked) {
		if (idList.length > 0) idList += ",";
		idList += checkBox.value.toString();
	}
}

function iterateCheckboxes(parentId, callback) {
	var prnt = document.getElementById(parentId);
	if (!prnt) return;

	var els = prnt.getElementsByTagName("div");
	var elsLen = els.length;
	var pattern = new RegExp('(^|\\s)(attr|sub-attr)(\\s|$)');
	for (i = 0, j = 0; i < elsLen; i++) {
		if (pattern.test(els[i].className)) {
			var chk, lbl, chks = els[i].getElementsByTagName("input");
			lbls = els[i].getElementsByTagName("label");
			if (chks && chks.length > 0) chk = chks[0];
			if (lbls && lbls.length > 0) lbl = lbls[0];
			if (lbl && chk) callback(chk, lbl.innerHTML);
		}
	}
}

// checks/unchecks all checkboxes for versions of this attribute (Scroll View)
function onVersionCheck(parentId, versionid, name, checked) {

	if (AC_ClientId) {
		var txt = document.getElementById(AC_ClientId + "_FreeText");
		if (txt) {
			if (txt.value == WatermarkTxt)
				txt.value = '';
			txt.value = txt.value.replace(/[\s]+/g, " ");
			if (checked) {
				if ((', ' + txt.value + ',').indexOf(', ' + name + ',') < 0) txt.value = txt.value ? txt.value + ", " + name : name;
			} else {
				var i = (', ' + txt.value + ',').indexOf(', ' + name + ',');
				if (i >= 0) txt.value = txt.value.substr(0, i) + txt.value.substr(i+2 + name.length);
			}
			txt.value = txt.value.replace(/^[,\s\xA0]+/, "").replace(/[,\s\xA0]+$/, "");

			UpdateWatermark(txt, WatermarkTxt);
		}
	}

	if (parentId<=0) return;
	var i,divs = document.getElementsByTagName("div"); 
	if (!divs) return;
	
	for(i=0;i<divs.length;++i) {
		if (divs[i] && divs[i].attributes["parent"] && 
			divs[i].attributes["parent"].value == parentId) {
			
			divs[i].style.display=(checked)?"block":"none";
			
//			var inp=divs[i].getElementsByTagName("input");
//			if (inp && inp.length > 0) {
//				//var span = divs[i].getElementsByTagName("span");
//				//if (span && span.length > 0 && span[0].attributes["versionId"])
//				if (inp[0] && inp[0].attributes["versionId"]) {
//					inp[0].checked = checked;
//                  onSelectAttr(span[0].innerHTML, checked);
//              }
//			}
		}
	}
}

// If an attribute is checked/unchecked then check/uncheck all its versions as well (Open View)
function checkAllVersions(checkbox, id) {
	var parentNode = document; //.getElementById("attributeListTable");
	if (parentNode)
	{
		var versions = parentNode.getElementsByTagName("input");
		if (!versions) return;
		for (var i=0; i<versions.length; i++) {
			var elm=versions[i];
			if (elm && elm.attributes["parentId"] && elm.type=="checkbox" && elm.attributes["parentId"].value==id) {
				if (elm.attributes["versionId"]) {
					var versionId = elm.attributes["versionId"].value;
					elm.checked = checkbox.checked;
				}
			}
		}
	}
}

function getAttrBarCheckboxes(searchClass, selectedOnly) {
	var classElements = new Array();
	var els = document.getElementsByTagName("div");
	var elsLen = els.length;
	var pattern = new RegExp('(^|\\s)'+searchClass+'(\\s|$)');
	for (i = 0, j = 0; i < elsLen; i++) {
		if ( pattern.test(els[i].className) ) {
			var subelms = els[i].getElementsByTagName("input");
			for (var k = 0; k < subelms.length; k++) {
				if (!selectedOnly || subelms[k].checked) classElements[j++] = subelms[k];
			}
		}
	}
	return classElements;
}

// Checks attribute version check boxes given a list of attribute versions
function SetAttrBarChecks(CheckBoxClass, versionList)
{
	var chkelms = getAttrBarCheckboxes(CheckBoxClass, false);
	for(var i = 0; i < chkelms.length; i++)
		chkelms[i].checked = false;

	if (versionList) {
		var attrIds = versionList.split(",");
		for(var i = 0; i < attrIds.length; i++)
		{
			for(var j = 0; j < chkelms.length; j++)
				if (chkelms[j].value.toString() == attrIds[i])
					chkelms[j].checked = true;
		}
	}
	if (typeof(MakeSelect) != 'undefined') MakeSelect(0,"");
	if (typeof(OnCloseAttrBar) != 'undefined') OnCloseAttrBar();
}

// Checks versions based on the quick pick selector
function SetAttrBar(selector, CheckBoxClass)
{
	var selected = selector.options[selector.selectedIndex];
	SetAttrBarChecks(CheckBoxClass, selected ? selected.getAttribute("attrs") : null);
}

// Populate the free-text field and tag check-box list from the selected quick-pick.
function SetFreeTextForQuickPick(selector, url) {
	if (selector && url && AC_ClientId) {
		var txt = document.getElementById(AC_ClientId + "_FreeText");
		if (txt) {
			$.get(url + selector.value, function(data) {
				if (data && data.length < 100)
					txt.value = data;
			});
		}

		UpdateWatermark(txt, WatermarkTxt);
	}
}

// Initialize watermark on input element.
function InitWatermark(inputId, label) {
	var inputObj = $('#' + inputId);
	if (!inputObj) return;

	inputObj.blur(function() { UpdateWatermark(this, label); });
	inputObj.focus(function() { UpdateWatermark(this, label); });

	var text = $.trim(inputObj.val());
	if (text == '' || text == label) {
		inputObj.val(label);
		inputObj.addClass('watermark');
	}
	else if (text != '' && text != label)
		inputObj.removeClass('watermark');
}

// Update the watermark as text changes within the input element.
function UpdateWatermark(inputObj, label) {
	if (!inputObj) return;

	var text = $.trim($(inputObj).val());
	if (text == label && $(inputObj).hasClass('watermark')) {
		$(inputObj).val('');
		$(inputObj).removeClass('watermark');
	}
	else if (text == '' && !$(inputObj).hasClass('watermark')) {
		$(inputObj).val(label);
		$(inputObj).addClass('watermark');
	}
	else
		$(inputObj).removeClass('watermark');
}
