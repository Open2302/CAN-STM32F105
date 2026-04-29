<%
'strconnection="driver={mysql odbc 3.51 driver};database=nli194_coocoxdb;server=localhost;uid=root;password=123456;port=3306"
strconnection="driver={mysql odbc 3.51 driver};database=nli194_coocoxdb;server=174.121.129.194;uid=coocoxdb;password=coocox2010;port=3306"
set conn = server.createobject("adodb.connection") 
conn.open strconnection

  ''''''''''''''''''''''''''''''''
  '    Disconnect  database
  ''''''''''''''''''''''''''''''''
	Function Disconnect()
		conn.Close()
		Set conn = Nothing
	End Function


	Dim objRs,strSQL,id
	id = Request.QueryString("id")
	
	Set objRs = Server.CreateObject("Adodb.RecordSet")
	strSQL = "Select hits From down Where software_Id='"&id&"'"
	objRs.CursorLocation = 3
	objRs.open strSQL,conn
	hits=0
	if not objRs.eof then 
		
	while not objRs.eof  
	hits =hits+Cint(objRs(0))
	objRs.movenext
	wend

	response.Write hits
	 else
	 response.Write("0")
	 end if 
	 	
	
	objRs.Close()
	Set objRs = Nothing
	Disconnect()
%>
