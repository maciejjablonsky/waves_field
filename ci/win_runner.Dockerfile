# escape=`
FROM mcr.microsoft.com/windows/servercore/insider:10.0.20348.1

SHELL ["powershell", "-Command", "$ErrorActionPreference = 'Stop';"]

RUN [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; `
    iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))"

RUN choco install -y git

RUN Invoke-WebRequest -Uri https://github.com/actions/runner/releases/download/v2.301.1/actions-runner-win-x64-2.301.1.zip -OutFile actions-runner-win-x64.zip; `
    mkdir actions-runner; `
    tar -xzf ./actions-runner-win-x64.zip -C actions-runner;

COPY .vsconfig C:\TEMP\.vsconfig

RUN Invoke-WebRequest -Uri https://aka.ms/vs/17/release/vs_buildtools.exe -OutFile C:\TEMP\vs_buildtools.exe; `
    C:\TEMP\vs_buildtools.exe --quiet --wait --norestart --nocache `
    --installPath C:\BuildTools --config C:\TEMP\.vsconfig

# Set environment variables
ENV VULKAN_SDK_VERSION=1.3.280.0

ENV VULKAN_SDK_INSTALLER=C:\VulkanSDK-${VULKAN_SDK_VERSION}-Installer.exe

# Download the Vulkan SDK installer
ADD https://sdk.lunarg.com/sdk/download/${VULKAN_SDK_VERSION}/windows/VulkanSDK-${VULKAN_SDK_VERSION}-Installer.exe ${VULKAN_SDK_INSTALLER}

# Set up environment variables for Vulkan
ENV VULKAN_SDK=C:\VulkanSDK\${VULKAN_SDK_VERSION}
ENV VK_SDK_PATH=${VULKAN_SDK}

# Install Vulkan SDK in headless mode
RUN & $env:VULKAN_SDK_INSTALLER install --confirm-command --accept-licenses; `
    & $env:VULKAN_SDK\Bin\glslc --version; `
    Remove-Item -Path $env:VULKAN_SDK_INSTALLER

COPY entrypoint.ps1 /actions-runner/entrypoint.ps1

WORKDIR /actions-runner

ENTRYPOINT ["powershell", "-File", "entrypoint.ps1"]
