// JavaScript Document
    function SwapDisplay(objName,imgname)
    {
        var obj=document.getElementById(objName);
		    var imgobj=document.getElementById(imgname);
        if(obj.style.display=="none")
        {
            obj.style.display="block";   
			      imgobj.className="minus";
        }
        else
        {
            obj.style.display="none";
			      imgobj.className="plus";
        }
    }   
