var path = "TinyCount";
var set_refresh = "1000";

function Trim(){
	return this.replace(/\s+$|^\s+/g,"");
}
String.prototype.Trim = Trim;

function getObj(elementId){
	return document.getElementById(elementId);	
}

function getObjValue(elementId){
	if(getObj(elementId).value!=undefined){
		return getObj(elementId).value.Trim();
	}else{
		return "";
	}
}

function XmlDoc(){	//创建XMLHttpRequest对象
	if(window.XMLHttpRequest){
		var xmlDoc = new XMLHttpRequest();
		return xmlDoc;
	}else{
		var arr = ['MSXML2.XMLHTTP.5.0','MSXML2.XMLHTTP.4.0','MSXML2.XMLHTTP.3.0','MSXML2.XMLHTTP','MICROSOFT.XMLHTTP.1.0','MICROSOFT.XMLHTTP.1','MICROSOFT.XMLHTTP'];
	
		for (var i=0;i<arr.length;i++){
			try{
				xmlDoc = new ActiveXObject(arr[i]);
				return xmlDoc;
			}catch(e){}
		}
	}
}

var count;
/*
	############
	param id 数据库数据表Counts的id号
	param tag 标签，前台用来显示数据(点此次数)的标签id
	############
*/
function getcounts(id,tag) {
	var xhr = XmlDoc();
	xhr.open("GET", path + "/getc.asp?id="+id+"&r="+Math.random(), true);
	xhr.onreadystatechange=function(){
		if(xhr.readyState==4){
			if(xhr.status==200){
				getObj(tag).innerHTML = xhr.responseText;
				count = xhr.responseText;
			}else{
				getObj(tag).innerHTML = "failed,try!";
			}
		}
	}
	xhr.send(null);
}

// ############
//	param id 数据库数据表Counts的id号
// param tag 标签，前台用来显示数据(点此次数)的标签id
// ############
function setcounts(id,tag) {
	var xhr2 = XmlDoc();
	xhr2.open("POST", path + "/setc.asp?id="+id, true);
	xhr2.onreadystatechange=function(){
		if(xhr2.readyState==4){
			if(xhr2.status==200){
				count++;
			}
		}
	}
	xhr2.send(count);
	//setInterval('getcounts('+id+',"'+tag+'")',set_refresh);
}