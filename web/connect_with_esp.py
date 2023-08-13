import time
import pywifi
from pywifi import const


def find_esp_network(interface):
    wifi = pywifi.PyWiFi()
    iface = wifi.interfaces()[interface]
    iface.scan()
    time.sleep(2)
    scan_results = iface.scan_results()

    for result in scan_results:
        if result.ssid.startswith("ESP_"):
            return result

    return None


def get_connected_ssid(interface):
    wifi = pywifi.PyWiFi()
    iface = wifi.interfaces()[interface]
    status = iface.status()
    if status == pywifi.const.IFACE_CONNECTED:
        current_profile = iface.network_profiles()[0]
        return current_profile.ssid
    else:
        return None


def connect_to_network(scan_result, password=None):
    wifi = pywifi.PyWiFi()  # Create a PyWiFi object
    iface = wifi.interfaces()[0]  # Get the first wireless interface

    ssid = scan_result.ssid

    if password is None:
        # Connect to open network
        iface.disconnect()
        iface.connect(scan_result)
        print(f"Connected to open network '{ssid}'.")
        return True
    elif password:
        # Connect to password-protected network
        profile = pywifi.Profile()
        profile.ssid = ssid
        profile.auth = pywifi.const.AUTH_ALG_OPEN
        profile.akm.append(pywifi.const.AKM_TYPE_WPA2PSK)
        profile.cipher = pywifi.const.CIPHER_TYPE_CCMP
        profile.key = password

        iface.remove_all_network_profiles()
        tmp_profile = iface.add_network_profile(profile)
        iface.connect(tmp_profile)
        print(f"Connected to secured network '{ssid}'.")
        return True
    else:
        print(f"Network '{ssid}' is secured but no password provided.")
        return False


if __name__ == "__main__":
    wifi_interface = 0  # Change this index if you have multiple WiFi interfaces

    while True:
        esp_network = find_esp_network(wifi_interface)
        connected_network = get_connected_ssid(wifi_interface)
        if esp_network:
            ssid = esp_network.ssid
            password = None  # Replace with the actual password

            # print(f"Found ESP network: {ssid}")
            # print(f"Connected network: {connected_network}")
            if (ssid != connected_network):
                if connect_to_network(esp_network, password):
                    print("Connected to the ESP network.")
                    time.sleep(15)
                else:
                    print("Failed to connect to the ESP network.")
            else:
                print("Already connected")
        else:
            print("ESP network not found. Waiting...")

        time.sleep(5)  # Wait for 10 seconds before scanning again
