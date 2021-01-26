#!/urs/bin/python3
#author :- Swathy
#date :- 28 May 20
#usage :- python3 gui.py
##Graphical User Interface for Energy Router(ER)

# Redis installed with "pip3 install redis"
# Activate virtual env by "source ~/Documents/main_project/Energy_Internet/bin/activate"
import redis
import PySimpleGUI as sg
from PySimpleGUI import Window
from PySimpleGUI import Column

# def addrow(y, z):
#     r = redis.Redis(host='127.0.0.1', port=6379)
#     x1 = r.get(y).decode('utf-8')
#     z.add_row([y, x1])


def portPower(p):
    r = redis.Redis(host='127.0.0.1', port=6379)
    pow = r.get(p).decode('utf-8')
    return pow


imageGraph = [[sg.Image(filename="graph.png", size = (7, 4), background_color='blue', key='_GRAPH_')]]
table1 = [[1,portPower(1)], [2,portPower(2)], [3,portPower(3)], [4,portPower(4)], [5,portPower(5)], [6,portPower(6)], [7,portPower(7)], [8,portPower(8)]]
table2 = []
reply = redis.Redis(host='127.0.0.1', port=6379)
value = reply.get('ID').decode('utf-8')
sg.theme('LightBlue3')
headings1 = ['Port', 'Power(in W)']
headings2 = ['Peer Router IP']
# for i in ['1', '2', '3', '4', '5', '6', '7', '8']:
#     addrow(i, x)
soc = reply.get('SOC').decode('utf-8')
tarrif = reply.get('TARRIF').decode('utf-8')
cl = reply.get('CRITICAL_LOAD').decode('utf-8')
peers = reply.hgetall('PEER:IP:PORT')
for j in peers.keys():
    table2.append([j.decode('utf-8')])
inBoard = reply.lrange('IN_BOARD', 0, -1)
outBoard = reply.lrange('OUT_BOARD', 0, -1)
frame_layout1 = [[sg.Listbox(values=inBoard, size=(68, 50), key='_INBOARD_')]]
frame_layout11 = [[sg.Listbox(values=outBoard, size=(68, 50), key='_OUTBOARD_')]]
frame_layout0 = [[sg.ProgressBar(100, orientation='v', size=(20, 60), bar_color=('lightgreen', 'white'), key='_PROGBAR_'), sg.Text('SOC = ' + soc + '%', text_color='brown', font='Courier 16 bold', justification='centre',
                           key='_SOCVALUE_')]]
col1 = Column([[sg.Text('Power Sharing', text_color='darkblue', font='Courier 14 underline bold')],
               [sg.Table(values=table1, headings=headings1, auto_size_columns=True,
                    display_row_numbers=False, num_rows=8, enable_events=True,
                    justification='centre', background_color='white', font='Courier 12', text_color='blue', key='_TABLE1_')],
               [sg.Text('CRITICAL LOAD = ' + cl + 'Ws', text_color='darkblue', font='Courier 14 bold', key='_CLOAD_')]])
col2 = Column([[sg.Frame('Energy Storage Level', frame_layout0, font='Courier 14 underline bold', title_color='darkblue',
                         element_justification='centre')], [
                   sg.Text('Tarrif = ' + tarrif + ' Rs', text_color='darkblue', font='Courier 16 bold', justification='centre',
                           key='_TARRIF_')]])
col4 = Column([[sg.Text('Set SOC value here:')], [sg.Input(key='-IN-', enable_events=True)], [sg.Button('Set')]])
tab1_layout = [[sg.Text('Peer Routers', text_color='darkblue', font='Courier 14 underline bold')],
               [sg.Table(values=table2, headings=headings2, auto_size_columns=True,
                    display_row_numbers=False, num_rows=3, enable_events=True,
                    justification='centre', background_color='white', font='Courier 12', key='_TABLE2_'), sg.Frame('Graph View', imageGraph, font='Courier 14 underline bold', title_color='darkblue')], [col1, col2, col4],
               [sg.Frame('RECEIVED MESSAGES', frame_layout1, font='Any 12', title_color='darkblue'), sg.Frame('SENT MESSAGES', frame_layout11, font='Any 12', title_color='darkblue')]]
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
    in range(10)]
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
window = Window(value, layout, resizable=True)


while True:
    event, values = window.read(timeout=900)
    # del x
    # del inBoard, outBoard, requestBoard, availabilityBoard, startBoard, stopBoard, startAckBoard, stopAckBoard
    inBoard = reply.lrange('IN_BOARD', 0, -1)
    outBoard = reply.lrange('OUT_BOARD', 0, -1)
    requestBoard = reply.lrange('REQ_BOARD_H', 0, -1)
    availabilityBoard = reply.lrange('AVL_BOARD_H', 0, -1)
    startBoard = reply.lrange('SRT_BOARD_H', 0, -1)
    stopBoard = reply.lrange('STP_BOARD_H', 0, -1)
    requestAckBoard = reply.lrange('ACK_REQ_BOARD_H', 0, -1)
    startAckBoard = reply.lrange('ACK_SRT_BOARD_H', 0, -1)
    stopAckBoard = reply.lrange('ACK_STP_BOARD_H', 0, -1)
    table1 = [[1,portPower(1)], [2,portPower(2)], [3,portPower(3)], [4,portPower(4)], [5,portPower(5)], [6,portPower(6)], [7,portPower(7)], [8,portPower(8)]]
    window['_TABLE1_'].update(values=table1)
    window['_INBOARD_'].update(values=inBoard)
    window['_OUTBOARD_'].update(values=outBoard)
    window['_REQUESTBOARD_'].update(values=requestBoard)
    window['_AVLBOARD_'].update(values=availabilityBoard)
    window['_SRTBOARD_'].update(values=startBoard)
    window['_STPBOARD_'].update(values=stopBoard)
    window['_ACKREQBOARD_'].update(values=requestAckBoard)
    window['_ACKSRTBOARD_'].update(values=startAckBoard)
    window['_ACKSTPBOARD_'].update(values=stopAckBoard)
    window['_GRAPH_'].Update(filename="graph.png")
    value = reply.get('SOC').decode('utf-8')
    soc = float(value)
    if soc >= 70:
        c = 'green'
    elif soc <= 40:
        c = 'red'
    else:
        c = 'blue'
    tarrif = reply.get('TARRIF').decode('utf-8')
    window['_TARRIF_'].update('Tarrif = ' + tarrif + ' Rs')
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
        f = open("/home/swathy/Documents/main_project/Energy_Internet/main/log_files/soc.log", "w")
        f.write(str(socVal))
        f.close()
        reply.lpush('SOC_IN', socVal)

window.close()
