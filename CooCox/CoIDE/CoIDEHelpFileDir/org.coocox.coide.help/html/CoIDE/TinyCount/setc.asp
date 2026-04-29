<%
'strconnection="driver={mysql odbc 3.51 driver};database=nli194_coocoxdb;server=localhost;uid=root;password=123456;port=3306"
strconnection="driver={mysql odbc 3.51 driver};database=nli194_coocoxdb;server=174.121.129.194;uid=coocoxdb;password=coocox2010;port=3306"
set conn = server.createobject("adodb.connection") 
conn.open strconnection
function getymd(str,flag)
						dim y,m,d,h,f,s,tempstr
							y = year(str):m = month(str):d = day(str)
							h = hour(str):f = minute(str):s = second(str)
						if len(m)=1 then m="0"&m:if len(d)=1 then d="0"&d
						if len(h)=1 then h="0"&h:if len(f)=1 then f="0"&f
						if len(s)=1 then s="0"&s
						select case flag
							case 0
								tempstr = y &"-"& m &"-"& d &" "& h &":"& f
							case 1
								tempstr = y &"-"& m &"-"& d
							case 2
								tempstr = m &"-"& d 
							case 3
								tempstr = h &":"& f
							case 4
								tempstr = y &"-"& m &"-"& d &" "& h &":"& f & ":" & s
							case 5
								tempstr = y &"年"& m &"月"& d &"日 "& h &":"& f &":"& s
							case 6
								tempstr = m &"月 ("& h &":"& f &")"
							case 7
								tempstr = y &"年"& m &"月"& d &"日"
							case 8
								tempstr = h &":"& f&":"& s
							case 9
								tempstr = m &"月"& d &"日"
							case 10
								tempstr = m &"月"
							case 11
								tempstr = m &"."& d &"."& right(y,2)
						end select
						getymd=tempstr
end function
	
  ''''''''''''''''''''''''''''''''
  '    Disconnect  database
  ''''''''''''''''''''''''''''''''
	Function Disconnect()
		conn.Close()
		Set conn = Nothing
	End Function

	Dim objRs,id,c,LastVersion,UpdateVersion,DownType,SoftwareID
	id = Request.QueryString("id")
	LastVersion = Request.QueryString("oldversion")
	UpdateVersion = Request.QueryString("newversion")
	DownType = Request.QueryString("type")
	
	if DownType="center" then
		if LastVersion="0" or LastVersion=0 then
			DownType="All"
		else
			DownType="Update"
		end if
	
	else
		DownType="Direct"
	end if 

	ip=request.servervariables("remote_addr")
	ipconvert=server.CreateObject("adodb.recordset")
	ipconvertsql="SELECT inet_aton('"&ip&"')"
	set ipconvert=conn.execute(ipconvertsql)
	ipnum=ipconvert(0)
								
	iprs=server.CreateObject("adodb.recordset")
	ipsql="SELECT country,id FROM country WHERE inet_aton('"&ip&"') >= StartIP AND inet_aton('"&ip&"') <= EndIP limit 1"
	set iprs = conn.execute(ipsql)
	if not iprs.bof then 
			do while not iprs.eof 
			country=iprs(0)
			countryid=iprs(1)
			iprs.movenext 
			loop 
	 else
			ww = Split(ip,".", -1, 1)
			qsh1=cint(ww(0))
			qsh2=cint(ww(1))
			qsh3=cint(ww(2))
			qsh4=cint(ww(3))
			'					 
			StartIP=(qsh1*256*256*256)+(qsh2*256*256)+(qsh3*256)+0
			EndIP=(qsh1*256*256*256)+(qsh2*256*256)+(qsh3*256)+255
			set addiprs = conn.execute("insert into country (StartIP,EndIP,code2,code3,country,State) values ("&StartIP&","&EndIP&",'unknown','unknown','unknown','unknown')")
			set crs=conn.execute("SELECT country,id FROM country WHERE inet_aton('"&ip&"') >= StartIP AND inet_aton('"&ip&"') <= EndIP limit 1")
			country=crs(0)
			countryid=Cint(crs(1))
	 end if 


	Set objRs = Server.CreateObject("Adodb.RecordSet")
	strSQL =" SELECT id,hits FROM down WHERE ip="&ipnum&" and software_ID='"&id&"'"
	objRs.open strSQL,Conn,1,3
			

	if not objRs.eof  then
		userid=objRs(0)
		c = objRs("hits")
		c = c + 1
		objRs("hits") = c
		objRs.Update()
		daten=getymd(now(),4)
		set downrecord = conn.execute("insert into downrecord  (ip,software_id,country_id,adddate,LastVersion,UpdateVersion,DownType) values ('"&ipnum&"','"&id&"','"&countryid&"','"&daten&"','"&LastVersion&"','"&UpdateVersion&"','"&DownType&"')") 
	else
		daten=getymd(now(),4)
		set ipaddrs = conn.execute("insert into down  (ip,software_id,country_id,adddate,hits) values ('"&ipnum&"','"&id&"','"&countryid&"','"&daten&"',1)")  
		set downrecord = conn.execute("insert into downrecord  (ip,software_id,country_id,adddate,LastVersion,UpdateVersion,DownType) values ('"&ipnum&"','"&id&"','"&countryid&"','"&daten&"','"&LastVersion&"','"&UpdateVersion&"','"&DownType&"')") 

		set useridrs=conn.execute("select id from down where ip="&ipnum&" ")
		userid=useridrs("id")
	end if
	
	objRs.Close()
	Set objRs = Nothing

		
			

%>