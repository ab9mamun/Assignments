from urllib import request

goog_url = 'https://textfiles.com/apple/ADC/adv.1988.index'


def download_stock_data(csv_url):
    response = request.urlopen(csv_url)
    csv = response.read()
    csv_str = str(csv)
    lines = csv_str.split("\\n")
    dest_url = r'goog.txt'
    fx = open(dest_url, "w")
    for line in lines:
        fx.write(line + "\n")
    fx.close()


download_stock_data(goog_url)