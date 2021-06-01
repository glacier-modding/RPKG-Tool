using System;
using System.Collections.Generic;
using System.Reflection;
using rpkg.ResourceActions.Attributes;

namespace rpkg.ResourceActions
{
    static class ActionRegistry
    {
        public class ActionDescriptor
        {
            public string Description { get; }

            private readonly Type _actionType;

            internal ActionDescriptor(string description, Type actionType)
            {
                Description = description;
                _actionType = actionType;
            }

            public void PerformAction(string rpkgPath, string resourceHash)
            {
                // The whole point of creating a new instance here every time is
                // that an action might want to maintain its own state and perform
                // cleanup after its done.
                using (var action = Activator.CreateInstance(_actionType) as IAction)
                    action.Perform(rpkgPath, resourceHash);
            }
        }

        public class BulkActionDescriptor
        {
            public string Description { get; }

            private readonly Type _actionType;

            internal BulkActionDescriptor(string description, Type actionType)
            {
                Description = description;
                _actionType = actionType;
            }

            public void PerformAction(string rpkgPath, string resourceType)
            {
                // The whole point of creating a new instance here every time is
                // that an action might want to maintain its own state and perform
                // cleanup after its done.
                using (var action = Activator.CreateInstance(_actionType) as IBulkAction)
                    action.PerformBulk(rpkgPath, resourceType);
            }
        }

        private static readonly List<ActionDescriptor> _commonActions = new List<ActionDescriptor>();
        private static readonly List<BulkActionDescriptor> _bulkCommonActions = new List<BulkActionDescriptor>();
        private static readonly Dictionary<string, List<ActionDescriptor>> _resourceActions = new Dictionary<string, List<ActionDescriptor>>();
        private static readonly Dictionary<string, List<BulkActionDescriptor>> _bulkResourceActions = new Dictionary<string, List<BulkActionDescriptor>>();

        static ActionRegistry()
        {
            // Discover all actions for the different resource types.
            foreach (var type in Assembly.GetExecutingAssembly().GetTypes())
            {
                var commonActionAttr = type.GetCustomAttribute<CommonActionAttribute>();

                if (commonActionAttr != null)
                {
                    RegisterCommonAction(commonActionAttr, type);
                    continue;
                }

                var bulkCommonActionAttr = type.GetCustomAttribute<BulkCommonActionAttribute>();

                if (bulkCommonActionAttr != null)
                {
                    RegisterBulkCommonAction(bulkCommonActionAttr, type);
                    continue;
                }

                var resourceActionAttr = type.GetCustomAttribute<ResourceActionAttribute>();

                if (resourceActionAttr != null)
                {
                    RegisterResourceAction(resourceActionAttr, type);
                    continue;
                }

                var bulkResourceActionAttr = type.GetCustomAttribute<BulkResourceActionAttribute>();

                if (bulkResourceActionAttr != null)
                {
                    RegisterBulkResourceAction(bulkResourceActionAttr, type);
                    continue;
                }
            }
        }

        private static void RegisterCommonAction(CommonActionAttribute attr, Type actionType)
        {
            if (!typeof(IAction).IsAssignableFrom(actionType))
                return;

            _commonActions.Add(new ActionDescriptor(attr.Description, actionType));
        }

        private static void RegisterBulkCommonAction(BulkCommonActionAttribute attr, Type actionType)
        {
            if (!typeof(IBulkAction).IsAssignableFrom(actionType))
                return;

            _bulkCommonActions.Add(new BulkActionDescriptor(attr.Description, actionType));
        }

        private static void RegisterResourceAction(ResourceActionAttribute attr, Type actionType)
        {
            if (!typeof(IAction).IsAssignableFrom(actionType))
                return;

            if (!_resourceActions.ContainsKey(attr.ResourceType))
                _resourceActions.Add(attr.ResourceType, new List<ActionDescriptor>());

            _resourceActions[attr.ResourceType].Add(new ActionDescriptor(attr.Description, actionType));
        }

        private static void RegisterBulkResourceAction(BulkResourceActionAttribute attr, Type actionType)
        {
            if (!typeof(IBulkAction).IsAssignableFrom(actionType))
                return;

            if (!_bulkResourceActions.ContainsKey(attr.ResourceType))
                _bulkResourceActions.Add(attr.ResourceType, new List<BulkActionDescriptor>());

            _bulkResourceActions[attr.ResourceType].Add(new BulkActionDescriptor(attr.Description, actionType));
        }

        public static IEnumerable<ActionDescriptor> GetActionsForResourceType(string resourceType)
        {
            foreach (var action in _commonActions)
                yield return action;

            if (!_resourceActions.TryGetValue(resourceType, out var resourceActions))
                yield break;

            foreach (var resourceAction in resourceActions)
                yield return resourceAction;
        }

        public static IEnumerable<BulkActionDescriptor> GetBulkActionsForResourceType(string resourceType)
        {
            foreach (var action in _bulkCommonActions)
                yield return action;

            if (!_bulkResourceActions.TryGetValue(resourceType, out var resourceActions))
                yield break;

            foreach (var resourceAction in resourceActions)
                yield return resourceAction;
        }
    }
}
