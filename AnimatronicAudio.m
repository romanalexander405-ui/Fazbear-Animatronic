port = "COM3";   % change to your port
baud = 9600;

youtube1 = "https://youtube.com/shorts/MZgHZpEn95Q?feature=share";  % dark
youtube2 = "https://www.youtube.com/watch?v=YdmFyBTVT0o";  % joystick

if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

s = serialport(port, baud);
configureTerminator(s, "LF");
flush(s);

disp("Listening for Arduino...");

while true
    line = readline(s);
    line = strtrim(line);

    if ~isempty(line)
        fprintf("From Arduino: %s\n", line);
    end

    if strcmp(line, "PLAY1")
        disp("Dark → opening YouTube 1...");
        web(youtube1, "-browser");
    elseif strcmp(line, "PLAY2")
        disp("Joystick → opening YouTube 2...");
        web(youtube2, "-browser");
    end
end
