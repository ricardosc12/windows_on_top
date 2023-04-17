#include <wx/wx.h>
#include <wx/webview.h>
#include <wx/html/htmlwin.h>
#include <wx/string.h>
#include <string>
#include <windows.h>


int wait_click = false;
HWND hwnd;
int on_top = false;
wxWebView* webView;

LRESULT CALLBACK hk(int nc, WPARAM wp, LPARAM lp) {
    if (nc >= 0) {
        if (wp == WM_LBUTTONDOWN) {
            if (wait_click) {
                hwnd = GetForegroundWindow();
                char title[256];
                GetWindowTextA(hwnd, title, sizeof(title));
                wxString jsScript = "document.getElementById('h4').innerHTML = '" + (wxString)"Janela: " + (wxString)title + " ';";
                webView->RunScript(jsScript);
                wait_click = false;
            }

        }
    }
    return CallNextHookEx(NULL, nc, wp, lp);
}

class MyFrame : public wxFrame
{
public:
    
    MyFrame(const wxString& title)
        : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(500, 200))
    {
        webView = wxWebView::New(this, wxID_ANY);

        // Carrega o conteúdo HTML na janela wxWebView
        wxString htmlContent = R"(
        <!DOCTYPE html>
<html>
    <head>
        <style>
            *{
                box-sizing: border-box;
                overflow: hidden;
            }
            body {
                background-color: white;
                font-family: Arial, sans-serif;
                overflow: hidden;
                padding: 5px 5px;
            }
            h3, h4 {
                margin: 0;
            }
            #h4 {
                line-height: 30px;
                font-size: 12px;
            }
            button {
                all: unset;
                padding: 3px 5px;
                width: fit-content;
                background-color: rgb(168, 168, 168);
            }
            /* td {
                background-color: beige;
            } */
        </style>
    </head>

    <body>
        <table style="border-spacing: 10px; border-collapse: separate;">
            <tr>
              <td><button onclick="location.href='button://get_window'">Encontrar</button></td>
              <td><h4 style="width: 300px; display: block; text-overflow: ellipsis; white-space: nowrap; overflow: hidden;" id="h4">Nenhuma janela selecionada !</h4></td>
            </tr>
            <tr>
              <td colspan="2" style="text-align: left;"><button id="top" onclick="location.href='button://on_top'">Colocar no topo</button></td>
            </tr>
            <tr>
                <td colspan="2" style="text-align: left;"><p style="margin: 0;">Transparência</p></td>
              </tr>
            <tr>
                <td>
                    <div style="display: inline-block">
                        <span style="display: inline-block"><button onclick="location.href='button://decrease'" style="width: 30px; height: 30px; border-radius: 15px; text-align: center;">-</button></span>
                        <span style="display: inline-block"><p id="transparency-value" style="margin: 0; line-height: 35px;">100%</p></span>
                        <span style="display: inline-block"><button onclick="location.href='button://increase'" style="width: 30px; height: 30px; border-radius: 15px; text-align: center;">+</button></span>
                    </div>
                </td>
            </tr>
          </table>
    </body>
</html>
        )";
        //wxFile file("index.html");
//file.ReadAll(&htmlContent);
        webView->SetPage(htmlContent, "");

        // Define o tamanho da janela wxWebView
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(webView, 1, wxEXPAND);
        SetSizer(sizer);

        webView->Bind(wxEVT_WEBVIEW_NAVIGATING, &MyFrame::OnWebViewNavigating, this);

    };
    private:
        int transparency = 100;
        void OnWebViewNavigating(wxWebViewEvent& event)
        {
            // Verifica se o evento foi acionado por um clique em um botão HTML
            wxString url = event.GetURL();
           
            if (url.StartsWith("button://"))
            {
 
                wxString buttonId = url.AfterFirst('/');
                if (buttonId == "/increase/") {
                    if (transparency == 100) return;
                    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
                    int sucess = SetLayeredWindowAttributes(hwnd, 0, (255 * (transparency+10)) / 100, LWA_ALPHA);
                    if (sucess == 0) return;
                    transparency += 10;
                    wxString jsScript = "document.getElementById('transparency-value').innerHTML = '" + std::to_string(transparency) + " %';";
                    webView->RunScript(jsScript);
                }
                else if (buttonId == "/decrease/") {
                    if (transparency == 10) return;
                    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
                    int sucess = SetLayeredWindowAttributes(hwnd, 0, (255 * (transparency-10)) / 100, LWA_ALPHA);
                    if (sucess == 0) return;
                    transparency -= 10;
                    wxString jsScript = "document.getElementById('transparency-value').innerHTML = '" + std::to_string(transparency) + " %';";
                    webView->RunScript(jsScript);
                }
                else if (buttonId == "/get_window/") {
                    wait_click = true;
                }
                else if (buttonId == "/on_top/") {
                    if (on_top) {
                        UINT flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW;
                        int sucess = SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, flags);
                        if (sucess == 0) return;
                        wxString top = "Colocar no topo";
                        wxString jsScript = "document.getElementById('top').innerHTML = '" + (wxString)"Colocar no topo" + " ';";
                        webView->RunScript(jsScript);
                        on_top = false;
                    }
                    else {
                        UINT flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW;
                        int sucess = SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, flags);
                        if (sucess == 0) return;
                        wxString top = "Remover do topo";
                        wxString jsScript = "document.getElementById('top').innerHTML = '" + (wxString)"Remover do topo" + " ';";
                        webView->RunScript(jsScript);
                        on_top = true;
                    }

                
                }
                event.Veto();
            }
        }

};

class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        HHOOK hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, hk, NULL, 0);
        MyFrame* frame = new MyFrame("Windows On Top");
        frame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);