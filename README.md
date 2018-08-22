# BanId Fix

### Описание
Данное расширение предназначено для исправления ошибки при бане через banid.
<br>
Когда забаненный игрок подключается к серверу, его кикает, но при этом сервер продолжает считать, что игрок просто подключается. Так появляются 'привидения', которые занимают слот.
<h1 align="center"><img src="https://zizt.ru/uploads/monthly_2018_08/unknown.png.6b368c75a2a745bcd69b3ba3a8b762cb.png"/></h1>

### Установка
1. Скачать [архив](https://github.com/komashchenko/BanId_Fix/releases) с последней версией
2. Распаковать архив
3. Закинуть на сервер
4. Перезапустить сервер
5. Ввести команду `sm exts list` и убедиться, что расширение нормально работает

### Квары
Расширение добавляет квар `sv_banned_from_server_message`, который позволяет изменить сообщение, отображаемое кикнутому игроку.
<br>
Значение по умолчанию `#Valve_Reject_Banned_From_Server`
