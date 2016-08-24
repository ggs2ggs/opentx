# -*- coding: utf-8 -*-

# English language sounds configuration

from tts_common import filename, NO_ALTERNATE, PROMPT_SYSTEM_BASE, PROMPT_CUSTOM_BASE


systemSounds = []
sounds = []

for i in range(100):
    systemSounds.append((str(i), filename(PROMPT_SYSTEM_BASE + i)))
for i in range(9):
    systemSounds.append((str(100 * (i + 1)), filename(PROMPT_SYSTEM_BASE + 100 + i)))
for i, s in enumerate(["thousand", "and", "minus", "point"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 109 + i)))
    
array = [("volt", "volt0", NO_ALTERNATE), ("volts", "volt1", NO_ALTERNATE),
                ("amp", "amp0", NO_ALTERNATE), ("amps", "amp1", NO_ALTERNATE),
                ("milliamp", "mamp0", NO_ALTERNATE), ("milliamps", "mamp1", NO_ALTERNATE),
                ("knot", "knot0", NO_ALTERNATE), ("knots", "knot1", NO_ALTERNATE),
                ("meter per second", "mps0", NO_ALTERNATE), ("meters per second", "mps1", NO_ALTERNATE),
                ("foot per second", "fps0", NO_ALTERNATE), ("feet per second", "fps1", NO_ALTERNATE),
                ("kilometer per hour", "kph0", NO_ALTERNATE), ("kilometers per hour", "kph1", NO_ALTERNATE),
                ("mile per hour", "mph0", NO_ALTERNATE), ("miles per hour", "mph1", NO_ALTERNATE),
                ("meter", "meter0", NO_ALTERNATE), ("meters", "meter1", NO_ALTERNATE),
                ("foot", "foot0", NO_ALTERNATE), ("feet", "foot1", NO_ALTERNATE),
                ("degree celsius", "celsius0", NO_ALTERNATE), ("degrees celsius", "celsius1", NO_ALTERNATE),
                ("degree fahrenheit", "fahr0", NO_ALTERNATE), ("degrees fahrenheit", "fahr1", NO_ALTERNATE),
                ("percent", "percent0", NO_ALTERNATE),
                ("milliamp-hour", "mamph0", NO_ALTERNATE), ("milliamp-hours", "mamph1", NO_ALTERNATE),
                ("watt", "watt0", NO_ALTERNATE), ("watts", "watt1", NO_ALTERNATE),
                ("milli-watt", "mwatt0", NO_ALTERNATE), ("milli-watts", "mwatt1", NO_ALTERNATE),
                ("db", "db0", NO_ALTERNATE),
                ("r p m", "rpm0", NO_ALTERNATE),
                ("g", "g0", NO_ALTERNATE),
                ("degree", "degree0", NO_ALTERNATE), ("degrees", "degree1", NO_ALTERNATE),
                ("radian", "rad0", NO_ALTERNATE), ("radians", "rad1", NO_ALTERNATE),
                ("milliliter", "ml0", NO_ALTERNATE), ("milliliters", "ml1", NO_ALTERNATE),
                ("fluid ounce", "founce0", NO_ALTERNATE), ("fluid ounces", "founce1", NO_ALTERNATE),
                ("hour", "hour0", NO_ALTERNATE), ("hours", "hour1", NO_ALTERNATE),
                ("minute", "minute0", NO_ALTERNATE), ("minutes", "minute1", NO_ALTERNATE),
                ("second", "second0", NO_ALTERNATE), ("seconds", "second1", NO_ALTERNATE),
                       ]
for i, (s, f, a) in enumerate(array):
    systemSounds.append((s, filename(f, PROMPT_SYSTEM_BASE + 115 + i)))
for i, s in enumerate(["point zero", "point one", "point two", "point three",
                       "point four", "point five", "point six",
                       "point seven", "point eight", "point nine"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 167 + i)))
for s, f, a in [(u"trim center", "midtrim", 495),
                (u"maximum trim reached", "endtrim", NO_ALTERNATE),
                (u"timer 1 elapsed", "timovr1", NO_ALTERNATE),
                (u"timer 2 elapsed", "timovr2", NO_ALTERNATE),
                (u"timer 3 elapsed", "timovr3", NO_ALTERNATE),
                (u"transmitter battery low", "lowbatt", 485),
                (u"inactivity alarm", "inactiv", 486),
                (u"throttle warning", "thralert", 481),
                (u"switch warning", "swalert", 482),
                (u"bad eeprom", "eebad", 483),
                (u"Welcome to open tea ex!", "hello", 480),
                (u"RF signal, low", "rssi_org", NO_ALTERNATE),
                (u"RF signal, critical", "rssi_red", NO_ALTERNATE),
                (u"radio antenna defective", "swr_red", NO_ALTERNATE),
                (u"telemetry lost", "telemko", NO_ALTERNATE),
                (u"telemetry recovered", "telemok", NO_ALTERNATE),
                (u"trainer signal lost", "trainko", NO_ALTERNATE),
                (u"trainer signal recovered", "trainok", NO_ALTERNATE),
                (u"sensor lost", "sensorko", NO_ALTERNATE),
                (u"servo overload", "servoko", NO_ALTERNATE),
                (u"power overload", "rxko", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))
for i, (s, f) in enumerate([(u"gear!, up!", "gearup"),
                            (u"gear!, down!", "geardn"),
                            (u"flaps!, up!", "flapup"),
                            (u"flaps!, down!", "flapdn"),
                            (u"spoiler!, up!", "splrup"),
                            (u"spoiler!, down!", "splrdn"),
                            (u"trainer!, on!", "trnon"),
                            (u"trainer!, off!", "trnoff"),
                            (u"engine!, off!", "engoff"),
                            (u"too. high!", "tohigh"),
                            (u"too. low!", "tolow"),
                            (u"low. battery!", "lowbat"),
                            (u"crow!, on!", "crowon"),
                            (u"crow!, off!", "crowof"),
                            (u"rf. signal!, low!", "siglow"),
                            (u"rf. signal!, critical!", "sigcrt"),
                            (u"high. speed. mode!, active", "spdmod"),
                            (u"thermal. mode!, on", "thmmod"),
                            (u"normal. mode!, on", "nrmmod"),
                            (u"landing. mode!, on", "lnding"),
                            (u"acro. mode!, on", "acro"),
                            (u"flight. mode!, one", "fm-1"),
                            (u"flight. mode!, two", "fm-2"),
                            (u"flight. mode!, three", "fm-3"),
                            (u"flight. mode!, four", "fm-4"),
                            (u"flight. mode!, five", "fm-5"),
                            (u"flight. mode!, six", "fm-6"),
                            (u"flight. mode!, seven", "fm-7"),
                            (u"flight. mode!, eight", "fm-8"),
                            (u"vario!, on", "vrion"),
                            (u"vario!, off", "vrioff"),
                            (u"flight mode!, power", "fm-pwr"),
                            (u"flight mode!, land", "fm-lnd"),
                            (u"flight mode!, float", "fm-flt"),
                            (u"flight mode!, speed", "fm-spd"),
                            (u"flight mode!, fast", "fm-fst"),
                            (u"flight mode!, normal", "fm-nrm"),
                            (u"flight mode!, cruise", "fm-crs"),
                            (u"flight mode!, acro", "fm-acr"),
                            (u"flight mode!, race", "fm-rce"),
                            (u"flight mode!, launch", "fm-lch"),
                            (u"flight mode!, ping", "fm-png"),
                            (u"flight mode!, thermal", "fm-thm"),
                            (u"flight mode!, thermal left", "fm-thml"),
                            (u"flight mode!, thermal right", "fm-thmr"),
                            ]):
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
