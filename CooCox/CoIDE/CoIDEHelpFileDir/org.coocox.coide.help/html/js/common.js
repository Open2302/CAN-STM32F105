// JavaScript Document
//show loading 
function loading()
{
	document.write("<br>");
	document.write("<br>");
	document.write("<br>");
	document.write("<br>");
	document.write("<br>");
	document.write("<br>");
	document.write("<html><div align=center id='load' >Web page to download and install support files, it may take a few minutes, please be patient.");  
	document.write("<br>");
	
	document.write(" Manually download the address：");
	document.write("<a  href='http://www.coocox.com/CoocoxUpdater/InstallFiles/Updater/Coocox Center.exe'id='download'>");
	document.write("Download CooCox Center</a>");  
	document.write("<br>");
	document.write("<img id=load src='images/loader.gif' /></div></html>");
	
 }

function getOs() 

{
 		loading();


		var urlInfo;
    	urlInfo=window.location.href;
	var intLen=urlInfo.length; 
	var offset=urlInfo.indexOf("?");  
	var strKeyValue=urlInfo.substr(offset,intLen);  
	var arrParam=strKeyValue.split("="); 
	var strParamValue=arrParam[1]; 


    var OsObject = ""; 
   if(navigator.userAgent.indexOf("MSIE")>0) { 
   		
   		 document.write("<script id=down src='test.js'><\/script>"); 
		document.write('<object ID="ATest1" classid="CLSID:1E80BC1A-066B-483B-9ACF-D967B9104EAA" codebase="http://www.coocox.com/CoocoxAX.cab#version=1,2,0,0" width=0 height=0">'); 
		 document.write('<param name="_Version" value=65536">');
		 document.write('<param name="_ExtentX" value=2646">');
		 document.write('<param name="_ExtentY" value=1323">');
		 document.write('<param name="_StockProps" value=0">');
		 document.write('</object>'); 
      	
		
		
		 checkuseronline(strParamValue);

        return "MSIE"; 
   } 
   if(isFirefox=navigator.userAgent.indexOf("Firefox")>0){ 
   
      	document.write("<script id=down src='test.js'><\/script>"); 
		document.write("<embed id='embed1'  type='application\/mozilla-npruntime-scriptable-plugin'  hidden='true'><\/embed>"); 
		var embed1 = document.getElementById('embed1');
	

        return "Firefox"; 
   } 
   if(isSafari=navigator.userAgent.indexOf("Safari")>0) { 
        return "Safari"; 
   }  
   if(isCamino=navigator.userAgent.indexOf("Camino")>0){ 
        return "Camino"; 
   } 
   if(isMozilla=navigator.userAgent.indexOf("Gecko/")>0){ 
        return "Gecko"; 
   } 
   
} 
 


//check email format 
function check()
{
var strm = document.userlogin.email.value   //??mail??????
 var regm =/^([a-zA-Z0-9_-])+@([a-zA-Z0-9_-])+((\.[a-zA-Z0-9_-]{2,3}){1,2})$/;//??Mail??????,^[a-zA-Z0-9_-]:???????,???,??,
 if(checkspace(document.userlogin.email.value) || document.userlogin.email.value.length < 1) {
	
    alert("Please enter email address!");
	 document.userlogin.email.focus();   
	return false;
  }
 if (!strm.match(regm) && strm!="")
   {
     alert('E-mail address format error or contain illegal characters! Please check!');
	 
  document.userlogin.email.focus();   
  return false;
    } 
 if(checkspace(document.userlogin.password.value) || document.userlogin.password.value.length < 6) {
	document.userlogin.password.focus();
    alert("Password length can not be less than 6, please re-enter!");
	 document.userlogin.password.focus();   
	return false;
  }
  
	document.userlogin.submit()
}

//check null  
function checkspace(checkstr) {
  var str = '';
  for(i = 0; i < checkstr.length; i++) {
    str = str + ' ';
  }
  return (str == checkstr);
}

//set cookies value
function SetCookie(name,value)//两个参数，一个是cookie的名子，一个是值
{
var Days = 30; //此 cookie 将被保存 30 天
var exp   = new Date(); //new Date("December 31, 9998");
exp.setTime(exp.getTime() + Days*24*60*60*1000);
document.cookie = name + "="+ escape (value) + ";expires=" + exp.toGMTString();
}

//read cookie value
function getCookie(name)//取cookies函数       
{
var arr = document.cookie.match(new RegExp("(^| )"+name+"=([^;]*)(;|$)"));
    if(arr != null) return unescape(arr[2]); return null;

}

//delete cookies value
function delCookie(name)//删除cookie
{
var exp = new Date();
exp.setTime(exp.getTime() - 1);
var cval=getCookie(name);
if(cval!=null) document.cookie= name + "="+cval+";expires="+exp.toGMTString();
}

//check cookies and reginfo 
  function checkuseronline(strParamValue)
  
  {
	
   	delCookie("userid")
  	if (getCookie("userid")==null || getCookie("userid")=="" || getCookie("userid")==" ")
	{ 
	
	
		if (ATest1.CCX_RUID()==null || ATest1.CCX_RUID()=="" || ATest1.CCX_RUID()==" ")
		{
			// alert('all none');
			 G("load").style.display = 'none';
			 popSignFlow(6);

		}
		else
		{
			//alert("cookies is null but reg is not null ");
			SetCookie ("userid", ATest1.CCX_RUID());
		//	alert("begin to down ");
			downfile(strParamValue);

		}
	}
    else
	{  
		//alert("cookies is not null but reg is  null ");

		var uid=null;
		ATest1.CCX_WUID(getCookie("userid"));
	//	alert("begin to down ");
	    downfile(strParamValue);
	}
	
  
  }
 
//click the download link

 function showloginpanel(linkid)
{
    var llogin = document.getElementById(linkid);
    if (document.all) 
	{
        llogin.click();
    } 
	else if (document.createEvent) 
	{
        var ev = document.createEvent('HTMLEvents');
        ev.initEvent('click', false, true);
        llogin.dispatchEvent(ev);
    }
}


// recept download parm and contact with activeX 
function downfile(strParamValue)
{
   if (strParamValue==null || strParamValue=="" || strParamValue==" ")
    {
		alert('Please choose the software which you want to download. ');
		history.go(-2);
		
	}
	else
	{	
	
		 if (ATest1.CCX_IsInsed('Center'))		
		{
			ATest1.CCX_Start('Center');
			ATest1.CCX_NewTask(strParamValue);
			
			G("load").style.display = 'none';
			//window.location.href='thank.asp';
		}
		else
		{	ATest1.CCX_NewTask(strParamValue);
			showloginpanel("download");
			//window.location.href='thank.asp';
			G("load").style.display = 'none';
			
		}
	}
}


//SHOW LOGIN PANNEL

function G(id){
    return document.getElementById(id);
};
function GC(t){
   return document.createElement(t);
};
String.prototype.trim = function(){
          return this.replace(/(^\s*)|(\s*$)/g, '');
};
function isIE(){
      return (document.all && window.ActiveXObject && !window.opera) ? true : false;
}function cancelSign(){
    G("sign_div").style.display = 'none';
    G("cover_div").style.display = 'none';
   document.body.style.overflow = '';
   window.location.href='user/login.htm';
};
function popCoverDiv(){
   if (G("cover_div")) {
    G("cover_div").style.display = '';
   } else {
    var coverDiv = GC('div');
    document.body.appendChild(coverDiv);
    coverDiv.id = 'cover_div';
    with(coverDiv.style) {
     position = 'absolute';
     background = '#CCCCCC';
     left = '0px';
     top = '0px';
     var bodySize = getBodySize();
     width = bodySize[0] + 'px'
     height = bodySize[1] + 'px';
     zIndex = 98;
     if (isIE()) {
      filter = "Alpha(Opacity=60)";
     } else {
      opacity = 0.6;
     }
    }
   }
}
function getBodySize(){
   var bodySize = [];
   with(document.documentElement) {
    bodySize[0] = (scrollWidth>clientWidth)?scrollWidth:clientWidth;
    bodySize[1] = (scrollHeight>clientHeight)?scrollHeight:clientHeight;
   }
   return bodySize;
}
function popSign(az_id){   var loginDivWidth = 400;
   var sign_in_flow = '<div style="background:#FFFFFF;"><h2>Sign in with your  Coocox Account&nbsp;&nbsp;</h2><form action="user/regcheck.asp" method="post" name="userlogin"  > </div><div align=left style="padding:8px 50px;">Your E-mail:'
       + '&nbsp;<input name="email" type="text" class="input-text" id="email" size="30" />'
       + '</div><div align=left style="padding:8px 50px;">&nbsp;&nbsp;Password:&nbsp;<input  id="password"  name="password" type="password" class="input-text" size="33" />'
        + '</div><div align=center  style="padding:8px"><input type="button" value="login" id="sign_button" onclick="check();"/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;'
        + '<input type="button" value="cancel" onclick="cancelSign();"/></div>'
		 + '<div style="padding:8px" style="text-decoration:none;color:#333333;font-size:12px;" >If you do not have any account, you just need to fill in the information, we will automatically creates it for you.  </form></div>';
		
		
		
		
   if (G("sign_div")) {
    G("sign_div").style.display = '';
   } else {
    var signDiv = GC('div');
    document.body.appendChild(signDiv);
    signDiv.id = 'sign_div';
    signDiv.align = "center";

    with (signDiv.style) {
     position = 'absolute';
     left = (document.documentElement.clientWidth - loginDivWidth)/2 + 'px';
     top = (document.documentElement.clientHeight - 300)/2 + 'px';
     width = loginDivWidth + 'px';
     zIndex = 99;
     background = '#FFFFFF';
     border = '#66CCFF solid 1px';
    }
   }    G("sign_div").innerHTML = sign_in_flow;


}
function popSignFlow(az_id) {
   popCoverDiv(); 
   popSign(az_id); 
   document.body.style.overflow = "hidden";
     
      
}




