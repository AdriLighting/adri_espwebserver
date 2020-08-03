#include <ButtonConstants.au3>
#include <EditConstants.au3>
#include <GUIConstantsEx.au3>
#include <WindowsConstants.au3>
#include <MsgBoxConstants.au3>
#Region ### START Koda GUI section ### Form=
$Form1 = GUICreate("Form1", 315, 233, 192, 124)
$Group1 = GUICtrlCreateGroup("", 8, 8, 297, 217)
$Input1 = GUICtrlCreateInput("9QeuqAtACdcvjIW", 24, 24, 177, 21)
$Button1 = GUICtrlCreateButton("conect socket", 207, 24, 91, 25)
$Edit1 = GUICtrlCreateEdit("", 24, 56, 273, 153)
GUICtrlSetData(-1, "")
GUICtrlCreateGroup("", -99, -99, 1, 1)
GUISetState(@SW_SHOW)
#EndRegion ### END Koda GUI section ###

While 1
    $nMsg = GUIGetMsg()
    Select
    case $nmsg = $button1
       tes()
        Case $nmsg =  $GUI_EVENT_CLOSE
            Exit
            endSelect
WEnd

func tes()
Local $sIPAddress = "wss://ws.192.168.0.78/" ; This IP Address only works for testing on your own computer.
    Local $iPort = guictrlread($input1) ; Port used for the connection.

    ; Assign a Local variable the socket and connect to a Listening socket with the IP Address and Port specified.
    Local $iSocket = TCPConnect($sIPAddress, $iPort)
    If @error Then
        ; The server is probably offline/port is not opened on the server.
        Local $iError = @error
        MsgBox(BitOR($MB_SYSTEMMODAL, $MB_ICONHAND), "", "Could not connect, Error code: " & $iError)
        Return False
    Else
        MsgBox($MB_SYSTEMMODAL, "", "Connection successful")
    EndIf

Local $iListenSocket = TCPListen($sIPAddress, $iPort, 100)

Local $iError = 0

    ; If an error occurred display the error code and return False.
    If @error Then
        ; Someone is probably already listening on this IP Address and Port (script already running?).
        $iError = @error
        MsgBox(BitOR($MB_SYSTEMMODAL, $MB_ICONHAND), "", "Could not listen, Error code: " & $iError)
        Return False
    EndIf

    ; Assign Local variable to be used by Listening and Client sockets.
    Local $iSocket = 0
    Do ; Wait for someone to connect (Unlimited).
        ; Accept incomming connexions if present (Socket to close when finished; one socket per client).
        $iSocket = TCPAccept($iListenSocket)

        ; If an error occurred display the error code and return False.
        If @error Then
            $iError = @error
            MsgBox(BitOR($MB_SYSTEMMODAL, $MB_ICONHAND), "", "Could not accept the incoming connection, Error code: " & $iError)
            Return False
        EndIf
    Until $iSocket <> -1 ;if different from -1 a client is connected.

    ; Close the Listening socket to allow afterward binds.
    TCPCloseSocket($iListenSocket)

    MsgBox($MB_SYSTEMMODAL, "", "Client Connected.")
















guictrlsetdata($edit1, $iListenSocket)


   EndFunc