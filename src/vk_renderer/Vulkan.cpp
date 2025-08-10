//
// Created by FullHat on 10/08/2025.
//
module;
#include <format>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_structs.hpp>
module Vulkan;

import Vulkan.Version;

void Vulkan::InitInstance()
{
    // input
    std::vector<const char*> additionalExtensions;

    vk::ApplicationInfo appInfo {
        .sType = vk::StructureType::eApplicationInfo,
        .pApplicationName = "Vulkan",
        .applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0),
        .pEngineName = "PieEngine",
        .engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 0),
        .apiVersion = vk::ApiVersion10,
    };

    vk::InstanceCreateInfo instanceCreateInfo {
        .sType = vk::StructureType::eInstanceCreateInfo,
        .pApplicationInfo = &appInfo
    };

    // get available extensions
    uint32_t extensionCount = 0;
    vk::Result err = vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    if (err != vk::Result::eSuccess) { throw std::runtime_error("failed to enumerate instance extensions"); }
    std::vector<vk::ExtensionProperties> extensions(extensionCount);
    err = vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    if (err != vk::Result::eSuccess) { throw std::runtime_error("failed to enumerate instance extensions"); }

    if (const uint32_t vkVersion = vk::enumerateInstanceVersion())
    {
        if (Version(vkVersion) > Version(0, 1, 3, 215).m_version)
        {
            additionalExtensions.emplace_back(vk::KHRPortabilityEnumerationExtensionName);
            instanceCreateInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
        }
    }

    logger->info("available extensions:");
    for (const auto& [extensionName, specVersion] : extensions)
    {
        logger->info(std::format("\tn: {}, v: {}",
            extensionName.data(),
            Version(specVersion).ToString()));
    }

    for (const auto& extension : additionalExtensions)
    {
        if (std::ranges::find_if(extensions,
            [&](const vk::ExtensionProperties props)-> bool
            {
                return strcmp(props.extensionName.data(), extension) == 0;
            }) == extensions.end())
        {
            throw std::runtime_error(std::format("one of the extensions is not available: {}", extension));
        }
    }

    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(additionalExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = additionalExtensions.data();

    if (vk::createInstance(&instanceCreateInfo, nullptr, &m_Instance) != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to create instance");
    }
    logger->info(std::format("instance created"));
}
