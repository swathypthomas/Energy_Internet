#!/urs/bin/python3
#author :- Swathy
#date :- 28 May 20
#usage :- python3 gui.py
##Graphical User Interface for Energy Router(ER)


import redis
import PySimpleGUI as sg
from PySimpleGUI import Window
from PySimpleGUI import Column


def portPower(p):
    r = redis.Redis(host='127.0.0.1', port=6379)
    pow = r.get(p).decode('utf-8')
    return pow


sg.theme('LightBrown2')    #$LightBlue3

#imageGraph = [[sg.Image(filename="graphF.png", size = (3.5, 2), background_color='brown', key='_GRAPH_')]]
imageGraph = [[sg.Image(filename="graph.png", size = (3.5, 2), background_color='brown', key='_GRAPH_')]]

table1 = [[1,portPower(1)], [2,portPower(2)], [3,portPower(3)], [4,portPower(4)], [5,portPower(5)], [6,portPower(6)], [7,portPower(7)], [8,portPower(8)]]
table2 = []
headings1 = ['Port', 'Power(in W)']
headings2 = ['Peer Router IP']

reply = redis.Redis(host='127.0.0.1', port=6379)
value = reply.get('ID').decode('utf-8')
soc = reply.get('SOC').decode('utf-8')
capacity = reply.get('CAPACITY').decode('utf-8')
tarrif = reply.get('TARRIF').decode('utf-8')
cl = reply.get('CRITICAL_LOAD').decode('utf-8')
peers = reply.hgetall('PEER:IP:PORT')
for j in peers.keys():
    table2.append([j.decode('utf-8')])
inBoard = reply.lrange('IN_BOARD', 0, -1)
outBoard = reply.lrange('OUT_BOARD', 0, -1)
iBi = []
oBi = []
for i in inBoard:
    p = i.decode('utf-8')
    iBi.append(p)
    iBi.append("                                      ")
for i in outBoard:
    p = i.decode('utf-8')
    oBi.append(p)
    oBi.append("                                      ")
    
row01 = [sg.Text('GRID INTERFACE', text_color='white', background_color='black', font='Courier 14 underline bold')]

frame11 = sg.Frame('Graph View', imageGraph, font='Courier 12 bold underline', background_color='white', title_color='brown')
col11 = Column([[frame11]])
frame_layout21 = [[sg.Table(values=table2, headings=headings2, auto_size_columns=True, display_row_numbers=False, num_rows=3, enable_events=True, justification='centre', background_color='white', font='Courier 10', key='_TABLE2_')]]
frame21 = sg.Frame('Peer routers IP list', frame_layout21, font='Courier 12 bold underline', background_color='white', title_color='brown', element_justification='centre')
frame_layout31 = [[sg.Table(values=table1, headings=headings1, auto_size_columns=True, display_row_numbers=False, num_rows=8, enable_events=True, justification='centre', background_color='white', font='Courier 10', key='_TABLE1_')]]
frame31 = sg.Frame('Power through ports ', frame_layout31, font='Courier 12 bold underline', background_color='white', title_color='brown', element_justification='centre')
col21 = Column([[frame21], [frame31]])
row02 = [col11, col21]

layer1 = [row01, row02]
F1 = [sg.Frame('',layer1)]

row03 = [sg.Text('ROUTER STATUS', text_color='white', background_color='black', font='Courier 14 underline bold')]

frame_layout12 =[
                 [sg.Text('SOC = ' + soc + '%', text_color='black', background_color='white', font='Courier 10 bold', justification='left', key='_SOCVALUE_')],
                 [sg.Text('TARRIF = ' + tarrif + 'Rs', text_color='black', background_color='white', font='Courier 10 bold', justification='left', key='_TARRIF_')],
                 [sg.Text('CRITICAL LOAD = ' + cl + 'Ws', text_color='black', background_color='white', font='Courier 10 bold', justification='left', key='_CLOAD_')],
                 [sg.Text('TOTAL CAPACITY = ' + capacity + 'Ws', text_color='black', background_color='white', font='Courier 10 bold', justification='left', key='_CAP_')]
                 #[sg.Text('', text_color='white', background_color='white', font='Courier 14 bold', justification='left')]        
                ]
frame12 = sg.Frame('Parameters', frame_layout12, font='Courier 12 bold underline', background_color='white', title_color='brown', element_justification='centre')
col12 = Column([[frame12]])
frame_layout22 = [[sg.ProgressBar(100, orientation='v', size=(9, 10), bar_color=('green', 'white'), border_width = 7, key='_PROGBAR_')]]
frame22 = sg.Frame('Energy storage level', frame_layout22, font='Courier 12 bold underline', background_color='white', title_color='brown', element_justification='centre')
col22 = Column([[frame22]])
row04 = [col12, col22]

layer2 = [row03, row04]
F2 = [sg.Frame('', layer2)]

row05 = [sg.Text('COMMUNICATION STATUS', text_color='white', background_color='black', font='Courier 14 underline bold')]

frame_layout13 =[[sg.Text(iBi, size=(28, 8), text_color='black', background_color='white', key='_OUTBOARD_')]]
frame13 = sg.Frame('Sent messages', frame_layout13, font='Courier 12 bold underline', background_color='white', title_color='brown', element_justification='centre')
col13 = Column([[frame13]], scrollable=True)
frame_layout23 =[[sg.Text(oBi, size=(28, 8), text_color='black', background_color='white', key='_INBOARD_')]]
frame23 = sg.Frame('Received messages', frame_layout23, font='Courier 12 bold underline', background_color='white', title_color='brown', element_justification='centre')
col23 = Column([[frame23]], scrollable=True)
row06 = [col13, col23]

layer3 = [row05, row06]
F3 = [sg.Frame('', layer3)]

row07 = [sg.Text('SIMULATION CONTROL', text_color='white', background_color='black', font='Courier 14 underline bold')]

frame_layout14 =[[sg.Input(key='-IN-', enable_events=True)], [sg.Button('Set')]]
frame14 = sg.Frame('Set SOC value here:', frame_layout14, font='Courier 12 bold underline', background_color='white', title_color='brown', element_justification='centre')
col14 = Column([[frame14]])
row08 = [col14]

layer4 = [row07, row08]
F4 = [sg.Frame('', layer4)]

layout1 = [#[sg.Text('', text_color='black', background_color='grey', font='Courier 14 bold', justification='centre')],
           F2, #[sg.Text('', text_color='black', background_color='grey', font='Courier 14 bold', justification='centre')],       
           F1, #[sg.Text('', text_color='white', background_color='grey', font='Courier 14 bold', justification='left')],
           F3, #[sg.Text('', text_color='white', background_color='grey', font='Courier 14 bold', justification='left')],
           F4]
tab1_layout = [[sg.Frame('', layout1)]]

requestBoard = reply.lrange('REQ_BOARD_H', 0, -1)
availabilityBoard = reply.lrange('AVL_BOARD_H', 0, -1)
startBoard = reply.lrange('SRT_BOARD_H', 0, -1)
stopBoard = reply.lrange('STP_BOARD_H', 0, -1)
requestAckBoard = reply.lrange('ACK_REQ_BOARD_H', 0, -1)
startAckBoard = reply.lrange('ACK_SRT_BOARD_H', 0, -1)
stopAckBoard = reply.lrange('ACK_STP_BOARD_H', 0, -1)
frame_layout2 = [[sg.Listbox(values=requestBoard, size=(68, 6), key='_REQUESTBOARD_')]]
frame_layout3 = [[sg.Listbox(values=requestBoard, size=(68, 6), key='_AVLBOARD_')]]
frame_layout4 = [[sg.Listbox(values=requestBoard, size=(68, 6), key='_SRTBOARD_')]]
frame_layout5 = [[sg.Listbox(values=requestBoard, size=(68, 6), key='_STPBOARD_')]]
frame_layout6 = [[sg.Listbox(values=requestBoard, size=(68, 6), key='_ACKREQBOARD_')]]
frame_layout7 = [[sg.Listbox(values=requestBoard, size=(68, 6), key='_ACKSRTBOARD_')]]
frame_layout8 = [[sg.Listbox(values=requestBoard, size=(68, 6), key='_ACKSTPBOARD_')]]
tab2_layout = [[sg.Frame('REQUESTS', frame_layout2, font='Any 12', title_color='blue')],
               [sg.Frame('AVAILABILITY', frame_layout3, font='Any 12', title_color='blue')],
               [sg.Frame('START TRANSFER', frame_layout4, font='Any 12', title_color='blue')],
               [sg.Frame('STOP TRANSFER', frame_layout5, font='Any 12', title_color='blue')],
               [sg.Frame('REQUEST ACK', frame_layout6, font='Any 12', title_color='blue')],
               [sg.Frame('START ACK', frame_layout7, font='Any 12', title_color='blue')],
               [sg.Frame('STOP ACK', frame_layout8, font='Any 12', title_color='blue')]]
headings = ['SL No.', 'From Router', 'To Router', 'From time', 'To time', 'Power']
header = [[sg.Text('  ')] + [sg.Text(h, size=(14, 1)) for h in headings]]
input_rows = [
    [sg.Input(size=(15, 1), pad=(0, 0)), sg.Input(size=(18, 1), pad=(0, 0)), sg.Input(size=(14, 1), pad=(0, 0)),
     sg.Input(size=(15, 1), pad=(0, 0)), sg.Input(size=(15, 1), pad=(0, 0)), sg.Input(size=(15, 1), pad=(0, 0))] for row
    in range(73)]
tab3_layout = header + input_rows
headings = ['SL No.', 'Due amount', 'Debt amount', 'Peer']
header = [[sg.Text('  ')] + [sg.Text(h, size=(14, 1)) for h in headings]]
input_rows = [
    [sg.Input(size=(15, 1), pad=(0, 0)), sg.Input(size=(18, 1), pad=(0, 0)), sg.Input(size=(14, 1), pad=(0, 0)),
     sg.Input(size=(15, 1), pad=(0, 0))] for row in range(10)]
tab4_layout = header + input_rows
col3 = [[sg.TabGroup([[sg.Tab('Home', tab1_layout)], [sg.Tab('Communication', tab2_layout)],
                      [sg.Tab('Transaction history', tab3_layout)], [sg.Tab('Billing', tab4_layout)]])]]
layout = [[sg.Column(col3, scrollable=True)]]
window = Window(value, layout, resizable=True, size=(600, 1000))


while True:
    event, values = window.read(timeout=900)
    # del x
    # del inBoard, outBoard, requestBoard, availabilityBoard, startBoard, stopBoard, startAckBoard, stopAckBoard
    inBoard = reply.lrange('IN_BOARD', 0, -1)
    outBoard = reply.lrange('OUT_BOARD', 0, -1)
    iB = []
    oB = []
    for i in inBoard:
        p = i.decode('utf-8')
        iB.append(p)
        iB.append("                                      ")
    for i in outBoard:
        p = i.decode('utf-8')
        oB.append(p)
        oB.append("                                      ")
    if set(iB) != set(iBi):
        iBi = iB
        window['_INBOARD_'].update(inBoard)
    if set(oB) != set(oBi):
        oBi = oB
        window['_OUTBOARD_'].update(outBoard)
    requestBoard = reply.lrange('REQ_BOARD_H', 0, -1)
    availabilityBoard = reply.lrange('AVL_BOARD_H', 0, -1)
    startBoard = reply.lrange('SRT_BOARD_H', 0, -1)
    stopBoard = reply.lrange('STP_BOARD_H', 0, -1)
    requestAckBoard = reply.lrange('ACK_REQ_BOARD_H', 0, -1)
    startAckBoard = reply.lrange('ACK_SRT_BOARD_H', 0, -1)
    stopAckBoard = reply.lrange('ACK_STP_BOARD_H', 0, -1)
    peers = reply.hgetall('PEER:IP:PORT')
    table2.clear()
    for j in peers.keys():
        if j not in table2:
            table2.append([j.decode('utf-8')])
    window['_TABLE2_'].update(values=table2)
    table1 = [[1,portPower(1)], [2,portPower(2)], [3,portPower(3)], [4,portPower(4)], [5,portPower(5)], [6,portPower(6)], [7,portPower(7)], [8,portPower(8)]]
    window['_TABLE1_'].update(values=table1)
    window['_REQUESTBOARD_'].update(values=requestBoard)
    window['_AVLBOARD_'].update(values=availabilityBoard)
    window['_SRTBOARD_'].update(values=startBoard)
    window['_STPBOARD_'].update(values=stopBoard)
    window['_ACKREQBOARD_'].update(values=requestAckBoard)
    window['_ACKSRTBOARD_'].update(values=startAckBoard)
    window['_ACKSTPBOARD_'].update(values=stopAckBoard)
    window['_GRAPH_'].Update(filename="graph.png")
    #window['_GRAPH_'].Update(filename="graphF.png")
    value = reply.get('SOC').decode('utf-8')
    soc = float(value)
    if soc >= 70:
        c = 'green'
    elif soc <= 40:
        c = 'red'
    else:
        c = 'blue'
    tarrif = reply.get('TARRIF').decode('utf-8')
    window['_TARRIF_'].update('TARRIF = ' + tarrif + 'Rs')
    window['_PROGBAR_'].update_bar(soc)
    window['_SOCVALUE_'].update('SOC = ' + str(value) + '%', text_color=c)
    cl = reply.get('CRITICAL_LOAD').decode('utf-8')
    window['_CLOAD_'].update('CRITICAL LOAD = ' + str(cl) + 'W')
    socVal = 0
    if event == '-IN-' and values['-IN-']:
        try:
            socVal = float(values['-IN-'])
        except:
            if len(values['-IN-']) == 1 and values['-IN-'][0] == '-':
                continue
            window['-IN-'].update(values['-IN-'][:-1])
    if event == 'Set':
        socVal = float(values['-IN-'])
        f = open("/home/swathy/Documents/main_project/Energy_Internet/main/log_files/soc.log", "w") #/home/pi/main_rpi/log_files for rPi
        f.write(str(socVal))
        f.close()
        reply.lpush('SOC_IN', socVal)

window.close()
