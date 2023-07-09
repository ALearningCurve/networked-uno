
import socket
import asyncio
import select
from concurrent.futures import ThreadPoolExecutor

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 1337  # The port used by the server
USER_INPUT_PREFIX = ""
isRunning = True

def stop():
    print("[!] Quit command recieved or some error ocurred. Exiting client momentarily...")
    global isRunning
    isRunning = False

async def nonBlockingInput():
    with ThreadPoolExecutor(1,'Async Input') as executor:
        return await asyncio.get_event_loop().run_in_executor(executor, lambda : input(USER_INPUT_PREFIX))

async def createUserInputHandler(s):
    loop = asyncio.get_running_loop()
    while isRunning:
        #close server with asynco inputs
        userInput = await nonBlockingInput()
        if not isRunning:
            return
        if userInput == "q":
            stop()
        else:
            await loop.sock_sendall(s, bytes(userInput, 'utf-8'))

async def createServerResponseHandler(s):
    loop = asyncio.get_running_loop()
    while isRunning: 
        try:
            data = await asyncio.wait_for(loop.sock_recv(s, 1024), timeout=3)
            if len(data) == 0:
                print("Error: Disconnected from server!")
                return stop()
            print(f"{data.decode('utf-8')}")
        except asyncio.TimeoutError:
            pass

async def main():
    print(f"[ ] Connecting to server on {HOST}:{PORT}")
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.setblocking(False)
        print(f"[ ] Connected to server on {HOST}:{PORT}")
        print(f"[ ] type \"q\" to quit the program")

        inputTask = asyncio.create_task(createUserInputHandler(s))
        await createServerResponseHandler(s)
        s.shutdown(2)


asyncio.run(main())